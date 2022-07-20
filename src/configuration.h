#pragma once
#include <string>
#include <inicpp/config.h>

class configuration {
public:
	configuration();

	void load_or_default(const std::string& filepath);
	void load(std::istream& strm);

	void save();
	void save(std::ostream& strm);
private:
	template <typename T>
	inline static bool get_ini_opt(const inicpp::section& section, const std::string& option, T& out);
public:
	std::string filepath;

	/* Section: Window */
	inicpp::signed_ini_t window_x, window_y;
	inicpp::unsigned_ini_t window_width, window_height;

	/* Section: Server */
	std::string user_agent;
};

template<typename T>
inline bool configuration::get_ini_opt(const inicpp::section& section, const std::string& option, T& out) {
	if (section.contains(option)) {
		const auto& opt = section[option];
		out = opt.get<T>();
		return true;
	}
	return false;
}
