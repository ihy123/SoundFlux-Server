#include <pch.h>
#include "configuration.h"
#include <inicpp/inicpp.h>
#include "util/utilities.h"

configuration::configuration() {
	auto wndRect = nana::API::make_center(400, 300);
	window_x = wndRect.x;
	window_y = wndRect.y;
	window_width = wndRect.width;
	window_height = wndRect.height;
}

void configuration::load_or_default(const std::string& filepath) {
	this->filepath = filepath;
	/* try to open config file for reading */
	std::fstream file(filepath);
	if (file.is_open()) {
		/* Parse file. Generate from scratch if an error occurs */
		try {
			load(file);
			return;
		}
		catch (const inicpp::exception&) {
			file.close();
		}
	}
	/* Generate file (put default config) */
	file.open(filepath, std::ios::out);
	if (file.is_open())
		save(file);
	else
		SF_LOG_WARN(LOG_BOTH, "Failed to open config file \"{}\". Configuration will not be saved.", filepath);
}

void configuration::load(std::istream& strm) {
	inicpp::config ini = inicpp::parser::load(strm);

	/* Section: Window */
	if (ini.contains("Window")) {
		const auto& sect = ini["Window"];
		get_ini_opt(sect, "X", window_x);
		get_ini_opt(sect, "Y", window_y);
		get_ini_opt(sect, "Width", window_width);
		get_ini_opt(sect, "Height", window_height);
	}

	/* Section: Server */
	if (ini.contains("Server")) {
		const auto& sect = ini["Server"];
		if (!get_ini_opt(sect, "ServerName", user_agent) || user_agent.empty())
			user_agent = get_default_user_agent_str();
	}
}

void configuration::save() {
	std::ofstream file(filepath);
	save(file);
}

void configuration::save(std::ostream& strm) {
	inicpp::config ini;

	/* Section: Window */
	inicpp::section sect_window("Window");
	sect_window.add_option("X", (inicpp::signed_ini_t)window_x);
	sect_window.add_option("Y", (inicpp::signed_ini_t)window_y);
	sect_window.add_option("Width", (inicpp::unsigned_ini_t)window_width);
	sect_window.add_option("Height", (inicpp::unsigned_ini_t)window_height);
	ini.add_section(sect_window);

	/* Section: Server */
	inicpp::section sect_server("Server");
	sect_server.add_option("ServerName", user_agent);
	ini.add_section(sect_server);

	inicpp::parser::save(ini, strm);
}
