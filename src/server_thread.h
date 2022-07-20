#pragma once
#include <thread>
#include <condition_variable>
#include <string>
#include <functional>
#include <ssdp-connect.h>
#include "IServerDataSupply.h"

class server_thread {
public:
	enum STATE {
		STATE_NOT_INITIALIZED, /* not initialized (still or already), call Start() to init */
		STATE_WAITING,         /* waits for a command */
		STATE_LISTENING,       /* listens for incoming connection with a client */
		STATE_WORKING,         /* exchanges data with client */
		STATE_STOPPING         /* stops the thread */
	};
public:
	server_thread(const std::string& service_type, const std::string& service_name, const std::string& user_agent);
	~server_thread() noexcept;

	/* Inits server's and SSDP sockets and creates the thread. Puts server in WAITING state.
	 * <tryout_ports> must be in network byte order */
	bool start(const unsigned short* tryout_ports, size_t num_ports);
	/* Join the thread and release SSDP and server sockets */
	bool join();

	/* Starts listening for connection with a client. When client sends valid handshake, a connection is established
	 * so callback is called. This->client is address of client.
	 * Puts server in LISTENING state. */
	void listen(std::function<void()> callback);

	/* Starts working phase: gets data from the supply and sends to client. Puts server in WORKING state. */
	void work(IServerDataSupply* _data_supply);

	/* Stop current operation and put thread in WAITING state */
	void stop();

	inline void set_user_agent(const std::string& val) { user_agent = val; }

	inline STATE get_state() const noexcept { return state; }
	inline unsigned short get_port() const noexcept { return server_port; }
private:
	void thread_proc();
	void change_state(STATE new_state);
	void listen_internal();
	void wait_internal();
	void work_internal();

	/* return -1 on critical error, 0 when connection hangs up, >0 on success */
	int process_incoming_data(const char* data, int size);
private:
	static constexpr char connection_handshake[] = "SoundFlux-837302";
	static constexpr int max_udp_packet = 576 - 20 - 8;

	STATE state;

	std::thread* thrd;

	/* for waiting */
	std::mutex wait_mtx;
	std::condition_variable wait_cond;

	ssdp_socket_t ssdp_socket;
	ssdp_socket_t server_socket;

	/* connection params */
	unsigned short server_port;
	sockaddr_in client;

	/* SSDP server parameters */
	std::string service_type;
	std::string service_name;
	std::string user_agent;

	/* for listening. called when a connection is established */
	std::function<void()> listenCallback;

	/* for working phase. Get data from the supply and send to client */
	IServerDataSupply* data_supply;

	/* true when state is changing */
	bool state_change;
};
