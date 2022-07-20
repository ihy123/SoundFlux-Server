#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include "server_thread.h"
#include "configuration.h"

class soundflux : public nana::form {
public:
	soundflux(const configuration& cfg);
private:
	static constexpr char service_type[] = "soundflux:server";
	static constexpr char service_name[] = "id:39232";

	nana::label port_label;
	nana::label addrs_label;
	nana::label user_agent_label;
	nana::textbox user_agent_textbox;
	nana::place layout;

	configuration config;
	server_thread server;
};
