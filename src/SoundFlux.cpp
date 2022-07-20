#include "pch.h"
#include "soundflux.h"
#include "util/global_log.h"

/* Ports to try when creating the server socket. 0 means pick any free port */
static const unsigned short tryout_ports[] = {
	htons(48765), htons(61324), htons(0)
};

using namespace nana;

int main(int argc, char* argv[]) {
	try {
		/* init log */
#ifdef NDEBUG
		global_log::init<true, false>();
#else
		setlocale(0, "");
		global_log::init<true, true>();
#endif
	}
	catch (const std::exception& e) {
		printf("Failed to init logger. Error message: %s\n", e.what());
		return -1;
	}

	try {
		configuration config;
		config.load_or_default("SoundFlux.ini");
		/* create main window and enter event loop */
		soundflux sf(config);
		sf.show();
		exec();
		return 0;
	}
	catch (const std::bad_alloc& e) {
		SF_LOG_FATAL(global_log::LOG_BOTH, "Memory allocation error. Error message: {}", e.what());
	}
	catch (const std::runtime_error& e) {
		SF_LOG_FATAL(global_log::LOG_DEFAULT, "A runtime error has occured: {}. Terminating", e.what());
	}
	return -1;
}

soundflux::soundflux(const configuration& cfg)
	: form(rectangle((int)cfg.window_x, (int)cfg.window_y, (unsigned int)cfg.window_width, (unsigned int)cfg.window_height),
		appearance(true, true, true, false, true, true, true)),
	server(service_type, service_name, cfg.user_agent), config(cfg) {
	caption("SoundFlux");

	/* save current config on exit */
	events().unload([this]() {
		nana::point p = pos();
		nana::size s = size();
		config.window_x = p.x;
		config.window_y = p.y;
		config.window_width = s.width;
		config.window_height = s.height;
		config.user_agent = user_agent_textbox.caption();
		config.save();
	});

	/* start server thread */
	if (!server.start(tryout_ports, sizeof(tryout_ports) / sizeof(*tryout_ports)))
		throw std::runtime_error("Failed to start server");

	/* start listening for connection */
	server.listen([this]() {
		/* when connection established */
		//server.work();
	});

	/* init controls */
	port_label.create(*this, true);
	addrs_label.create(*this, true);
	layout.bind(*this);
	user_agent_label.create(*this, true);
	user_agent_textbox.create(*this, true);

	/* display user agent name */
	user_agent_label.caption("Server name:");
	user_agent_textbox.caption(cfg.user_agent);
	user_agent_textbox.events().text_changed([this](const arg_textbox& arg) {
		server.set_user_agent(arg.widget.caption());
	});

	/* display port */
	char buf[12] = "Port: ";
	itoa(ntohs(server.get_port()), buf + 6, 10);
	port_label.caption(buf);

	/* display addresses */
	addrs_label.caption("Address: 192.168.0.31");

	layout.div("vert margin=[10,10,10,10] <weight=10% <weight=35% port><addrs>><weight=10% <weight=20% us_ag_l><us_ag>><>");
	layout["port"] << port_label;
	layout["addrs"] << addrs_label;
	layout["us_ag_l"] << user_agent_label;
	layout["us_ag"] << user_agent_textbox;
	layout.collocate();
}
