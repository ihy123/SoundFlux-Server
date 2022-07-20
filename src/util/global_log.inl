
template<bool enable_logfile, bool enable_stdout>
inline bool global_log::init() {
	if constexpr (enable_logfile)
		g_log.logger.sinks().push_back(
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("SoundFlux.log", true));

	if constexpr (enable_stdout)
		g_log.logger.sinks().push_back(
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

	g_log.logger.set_level(spdlog::level::trace);
	g_log.logger.set_pattern("%^[%n: %l] [%H:%M:%S:%e] [Thrd:%t] {%s:%#}%$ %v");

	return true;
}

template<global_log::LOG_METHOD m, typename ...T>
inline void global_log::log_debug(spdlog::source_loc sl, spdlog::format_string_t<T...> fmt, T && ...args) {
	if constexpr (m & LOG_DEFAULT)
		g_log.logger.log(sl, spdlog::level::debug, fmt, std::forward<T>(args)...);
	if constexpr (m & LOG_MSGBOX)
		(nana::msgbox("SoundFlux: DEBUG").icon(nana::msgbox::icon_information) << fmt::format(fmt, std::forward<T>(args)...)).show();
}

template<global_log::LOG_METHOD m, typename ...T>
inline void global_log::log_info(spdlog::source_loc sl, spdlog::format_string_t<T...> fmt, T&& ...args) {
	if constexpr (m & LOG_DEFAULT)
		g_log.logger.log(sl, spdlog::level::info, fmt, std::forward<T>(args)...);
	if constexpr (m & LOG_MSGBOX)
		(nana::msgbox("SoundFlux: INFO").icon(nana::msgbox::icon_information) << fmt::format(fmt, std::forward<T>(args)...)).show();
}

template<global_log::LOG_METHOD m, typename ...T>
inline void global_log::log_warn(spdlog::source_loc sl, spdlog::format_string_t<T...> fmt, T && ...args) {
	if constexpr (m & LOG_DEFAULT)
		g_log.logger.log(sl, spdlog::level::warn, fmt, std::forward<T>(args)...);
	if constexpr (m & LOG_MSGBOX)
		(nana::msgbox("SoundFlux: WARNING").icon(nana::msgbox::icon_warning) << fmt::format(fmt, std::forward<T>(args)...)).show();
}

template<global_log::LOG_METHOD m, typename ...T>
inline void global_log::log_error(spdlog::source_loc sl, spdlog::format_string_t<T...> fmt, T && ...args) {
	if constexpr (m & LOG_DEFAULT)
		g_log.logger.log(sl, spdlog::level::err, fmt, std::forward<T>(args)...);
	if constexpr (m & LOG_MSGBOX)
		(nana::msgbox("SoundFlux: ERROR").icon(nana::msgbox::icon_warning) << fmt::format(fmt, std::forward<T>(args)...)).show();
}

template<global_log::LOG_METHOD m, typename ...T>
inline void global_log::log_fatal(spdlog::source_loc sl, spdlog::format_string_t<T...> fmt, T && ...args) {
	if constexpr (m & LOG_DEFAULT)
		g_log.logger.log(sl, spdlog::level::critical, fmt, std::forward<T>(args)...);
	if constexpr (m & LOG_MSGBOX)
		(nana::msgbox("SoundFlux: FATAL ERROR").icon(nana::msgbox::icon_error) << fmt::format(fmt, std::forward<T>(args)...)).show();
}

template<global_log::LOG_METHOD m, typename ...T>
inline void global_log::errno_log_debug(int err, spdlog::source_loc sl, spdlog::format_string_t<int, char[256], T...> fmt, T && ...args) {
	char msg[256];
	strerror_s(msg, sizeof(msg), err);
	log_debug<m>(sl, fmt, std::forward<int>(err), std::forward<char[256]>(msg), std::forward<T>(args)...);
}

template<global_log::LOG_METHOD m, typename ...T>
inline void global_log::errno_log_info(int err, spdlog::source_loc sl, spdlog::format_string_t<int, char[256], T...> fmt, T&&... args) {
	char msg[256];
	strerror_s(msg, sizeof(msg), err);
	log_info<m>(sl, fmt, std::forward<int>(err), std::forward<char[256]>(msg), std::forward<T>(args)...);
}

template<global_log::LOG_METHOD m, typename ...T>
inline void global_log::errno_log_warn(int err, spdlog::source_loc sl, spdlog::format_string_t<int, char[256], T...> fmt, T && ...args) {
	char msg[256];
	strerror_s(msg, sizeof(msg), err);
	log_warn<m>(sl, fmt, std::forward<int>(err), std::forward<char[256]>(msg), std::forward<T>(args)...);
}

template<global_log::LOG_METHOD m, typename ...T>
inline void global_log::errno_log_error(int err, spdlog::source_loc sl, spdlog::format_string_t<int, char[256], T...> fmt, T && ...args) {
	char msg[256];
	strerror_s(msg, sizeof(msg), err);
	log_error<m>(sl, fmt, std::forward<int>(err), std::forward<char[256]>(msg), std::forward<T>(args)...);
}

template<global_log::LOG_METHOD m, typename ...T>
inline void global_log::errno_log_fatal(int err, spdlog::source_loc sl, spdlog::format_string_t<int, char[256], T...> fmt, T && ...args) {
	char msg[256];
	strerror_s(msg, sizeof(msg), err);
	log_fatal<m>(sl, fmt, std::forward<int>(err), std::forward<char[256]>(msg), std::forward<T>(args)...);
}
