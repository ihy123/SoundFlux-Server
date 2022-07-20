#include <pch.h>
#include "server_thread.h"
#include "util/net_error.h"

/* for poll() */
#ifdef SF_PLATFORM_WINDOWS
#define poll WSAPoll
#else
#include <sys/poll.h>
#endif

server_thread::server_thread(const std::string& service_type, const std::string& service_name, const std::string& user_agent)
	: state(STATE_NOT_INITIALIZED), thrd(nullptr), ssdp_socket(-1), server_socket(-1), server_port(0), client(),
	service_type(service_type), service_name(service_name), user_agent(user_agent), data_supply(nullptr), state_change(false) {}

server_thread::~server_thread() noexcept {
	join();
}

bool server_thread::start(const unsigned short* tryout_ports, size_t num_ports) {
	/* Init WinSock2 on Windows */
#ifdef SF_PLATFORM_WINDOWS
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		NET_LOG_FATAL(global_log::LOG_BOTH, "Failed to init WinSock2");
		return false;
	}
#endif

	/* create SSDP socket */
	ssdp_socket = ssdp_socket_init(1);
	if (ssdp_socket == -1)
		NET_LOG_ERROR(global_log::LOG_BOTH, "Failed to create SSDP socket");

	/* create server socket */
	server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server_socket == -1) {
		NET_LOG_FATAL(global_log::LOG_BOTH, "Failed to create server socket");
		return false;
	}

	/* set non-blocking */
#ifdef SF_PLATFORM_WINDOWS
	u_long nonblock = 1;
	if (ioctlsocket(server_socket, FIONBIO, &nonblock) == -1) {
#else
	int nonblock = 1;
	if (ioctl(server_socket, FIONBIO, &nonblock) == -1) {
#endif
		NET_LOG_FATAL(global_log::LOG_BOTH, "Failed to make server socket non-blocking");
		return false;
	}

	/* set reuse addr */
	int sockopt = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&sockopt, sizeof(sockopt)) == -1)
		NET_LOG_WARN(global_log::LOG_DEFAULT, "Failed to set an option for server socket SO_REUSEADDR");

	/* bind server socket */
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	size_t i = 0;
	for (; i < num_ports; ++i) {
		server_addr.sin_port = tryout_ports[i];
		if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0)
			break;
	}
	if (i == num_ports) {
		NET_LOG_FATAL(global_log::LOG_BOTH, "Failed to bind server socket");
		return false;
	}
	else if (server_addr.sin_port == tryout_ports[num_ports - 1] && ssdp_socket == -1)
		SF_LOG_ERROR(global_log::LOG_BOTH, "Server cannot be located automatically. Use manual connection.");

	/* query server's port */
	int namelen = sizeof(server_addr);
	if (getsockname(server_socket, (struct sockaddr*)&server_addr, &namelen) == -1)
		NET_LOG_WARN(global_log::LOG_BOTH, "Failed to query server's port number using getsockname()");
	server_port = server_addr.sin_port;

	/* start the thread and put it to WAITING state */
	state = STATE_WAITING;
	thrd = new std::thread(&server_thread::thread_proc, this);
	return true;
}

bool server_thread::join() {
	/* if thread is not running, do nothing */
	if (state == STATE_STOPPING)
		return false;

	change_state(STATE_STOPPING);
	if (thrd) {
		if (thrd->joinable())
			thrd->join();
		delete thrd;
		thrd = nullptr;
	}
	/* close sockets and release WinSock2 on Windows */
	ssdp_socket_release(server_socket);
	ssdp_socket_release(ssdp_socket);
	server_socket = ssdp_socket = -1;
#ifdef SF_PLATFORM_WINDOWS
	WSACleanup();
#endif
	state = STATE_NOT_INITIALIZED;
	server_port = 0;
	client = {0};
	state_change = false;
	return true;
}

void server_thread::listen(std::function<void()> callback) {
	listenCallback = std::move(callback);
	change_state(STATE_LISTENING);
}

void server_thread::work(IServerDataSupply* _data_supply) {
	data_supply = _data_supply;
	change_state(STATE_WORKING);
}

void server_thread::stop() {
	change_state(STATE_WAITING);
}

void server_thread::thread_proc() {
	while (true) {
		/* do action corresponding to current state value */
		switch (state) {
		case STATE_LISTENING:
			listen_internal();
			break;
		case STATE_WAITING:
			wait_internal();
			break;
		case STATE_WORKING:
			work_internal();
			break;
		case STATE_STOPPING:
			return;
		}

		/* if current action stopped, so the state changed, so we reset the flag */
		state_change = false;
	}
}

void server_thread::change_state(STATE new_state) {
	if (state != new_state) {
		STATE prev_state = state;
		state = new_state;
		state_change = true;
		if (prev_state == STATE_WAITING)
			wait_cond.notify_one();
	}
}

void server_thread::listen_internal() {
	/* for socket I/O */
	int result = 0;
	char buffer[512] = { 0 };
	sockaddr_in from;
	int fromsize = sizeof(from);

	/* request information */
	SSDP_REQUEST_TYPE req_type;
	char req_svc_type[128];

	/* pollfd for poll() */
	struct pollfd pfd[2];
	pfd[0].fd = ssdp_socket;
	pfd[1].fd = server_socket;
	pfd[1].events = pfd[0].events = POLLIN;

	while (result >= 0 && !state_change) {
		/* wait on SSDP and server socket */
		result = poll(pfd, 2, 20);
		if (result <= 0)
			continue;
		/* receive requests and respond */
		if (pfd[0].revents & POLLIN) {
			pfd[0].revents = 0;
			result = recvfrom(ssdp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &fromsize);
			if (result < 0)
				break;
			result = ssdp_parse_request(buffer, result, &req_type, req_svc_type, sizeof(req_svc_type), NULL, 0, NULL, 0);
			if (result > 0 && req_type == SSDP_RT_DISCOVER && strncmp(req_svc_type, service_type.c_str(), service_type.length()) == 0) {
				result = ssdp_response(service_type.c_str(), service_name.c_str(), user_agent.c_str(), buffer, sizeof(buffer));
				result = sendto(server_socket, buffer, result, 0, (struct sockaddr*)&from, sizeof(from));
				if (result < 0)
					break;
			}
			memset(buffer, 0, sizeof(buffer));
		}
		/* receive data on server socket and forward it to the callback */
		if (pfd[1].revents & POLLIN) {
			pfd[1].revents = 0;
			result = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &fromsize);
			if (result < 0)
				break;
			else if (result == 0)
				continue;
			else if (result == (sizeof(connection_handshake) - 1) &&
				memcmp(connection_handshake, buffer, (sizeof(connection_handshake) - 1)) == 0) {
				client = from;
				break;
			}
			memset(buffer, 0, sizeof(buffer));
		}
	}

	if (result < 0)
		NET_LOG_FATAL(global_log::LOG_BOTH, "An error has occured while server was listening.");
	else
		listenCallback();
}

void server_thread::wait_internal() {
	std::unique_lock lock(wait_mtx);
	while (!state_change)
		wait_cond.wait(lock);
}

void server_thread::work_internal() {
	/* for socket I/O */
	int result = 0;
	char recvbuf[512] = { 0 };
	sockaddr_in from;
	int fromsize = sizeof(from);

	char sendbuf[max_udp_packet];

	/* pollfd for poll() */
	pollfd pfd {
		.fd = server_socket,
		.events = POLLIN | POLLOUT
	};

	while (result >= 0 && !state_change) {
		/* wait to send and receive on server socket */
		result = poll(&pfd, 1, -1);
		if (result <= 0)
			continue;
		/* send data */
		if (pfd.revents & POLLOUT) {
			pfd.revents &= ~short(POLLOUT);
			result = data_supply->GetData(sendbuf, max_udp_packet);
			result = sendto(server_socket, sendbuf, result, 0, (struct sockaddr*)&client, sizeof(client));
			if (result < 0)
				break;
		}
		/* receive data and process it */
		if (pfd.revents & POLLIN) {
			pfd.revents &= ~short(POLLIN);
			result = recvfrom(server_socket, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&from, &fromsize);
			if (result < 0)
				break;
			else if (result == 0)
				continue;
			if (process_incoming_data(recvbuf, result) <= 0)
				break;
			memset(recvbuf, 0, result);
		}
	}

	if (result < 0)
		NET_LOG_FATAL(global_log::LOG_BOTH, "A networking error has occured on the server.");
}

int server_thread::process_incoming_data(const char* data, int size) {
	return 1;
}
