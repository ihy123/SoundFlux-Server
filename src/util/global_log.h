#pragma once

#ifdef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

class global_log {
public:
	enum LOG_METHOD {
		LOG_DEFAULT = 1,                     /* log to file (default) */
		LOG_MSGBOX = 2,                      /* show message box */
		LOG_BOTH = LOG_DEFAULT | LOG_MSGBOX  /* log to file and show message box */
	};
public:
	template <bool enable_logfile = true, bool enable_stdout = false>
	static bool init();

	template <LOG_METHOD m = LOG_DEFAULT, typename... T>
	inline static void log_debug(spdlog::source_loc sl, spdlog::format_string_t<T...> fmt, T&&... args);
	template <LOG_METHOD m = LOG_DEFAULT, typename... T>
	inline static void log_info(spdlog::source_loc sl, spdlog::format_string_t<T...> fmt, T&&... args);
	template <LOG_METHOD m = LOG_DEFAULT, typename... T>
	inline static void log_warn(spdlog::source_loc sl, spdlog::format_string_t<T...> fmt, T&&... args);
	template <LOG_METHOD m = LOG_DEFAULT, typename... T>
	inline static void log_error(spdlog::source_loc sl, spdlog::format_string_t<T...> fmt, T&&... args);
	template <LOG_METHOD m = LOG_DEFAULT, typename... T>
	inline static void log_fatal(spdlog::source_loc sl, spdlog::format_string_t<T...> fmt, T&&... args);

	template <LOG_METHOD m = LOG_DEFAULT, typename... T>
	inline static void errno_log_debug(int err, spdlog::source_loc sl, spdlog::format_string_t<int, char[256], T...> fmt, T&&... args);
	template <LOG_METHOD m = LOG_DEFAULT, typename... T>
	inline static void errno_log_info(int err, spdlog::source_loc sl, spdlog::format_string_t<int, char[256], T...> fmt, T&&... args);
	template <LOG_METHOD m = LOG_DEFAULT, typename... T>
	inline static void errno_log_warn(int err, spdlog::source_loc sl, spdlog::format_string_t<int, char[256], T...> fmt, T&&... args);
	template <LOG_METHOD m = LOG_DEFAULT, typename... T>
	inline static void errno_log_error(int err, spdlog::source_loc sl, spdlog::format_string_t<int, char[256], T...> fmt, T&&... args);
	template <LOG_METHOD m = LOG_DEFAULT, typename... T>
	inline static void errno_log_fatal(int err, spdlog::source_loc sl, spdlog::format_string_t<int, char[256], T...> fmt, T&&... args);

	inline static global_log& get() noexcept { return g_log; }
private:
	global_log();
private:
	static global_log g_log;

	spdlog::logger logger;
};

#define _SF_LOG_TEMPLATE(fun_suffix, method, ...) global_log::log_##fun_suffix<global_log::method>(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, __VA_ARGS__)
#define SF_LOG_DEBUG(method, ...) _SF_LOG_TEMPLATE(debug, method, __VA_ARGS__)
#define SF_LOG_INFO(method, ...) _SF_LOG_TEMPLATE(info, method, __VA_ARGS__)
#define SF_LOG_WARN(method, ...) _SF_LOG_TEMPLATE(warn, method, __VA_ARGS__)
#define SF_LOG_ERROR(method, ...) _SF_LOG_TEMPLATE(error, method, __VA_ARGS__)
#define SF_LOG_FATAL(method, ...) _SF_LOG_TEMPLATE(fatal, method, __VA_ARGS__)

#define _SF_ERRNO_LOG_TEMPLATE(fun_suffix, method, errno_val, fmt, ...) global_log::errno_log_##fun_suffix<global_log::method>(errno_val,\
	spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, "Errno: {} [{}] " fmt, __VA_ARGS__)
#define SF_ERRNO_LOG_DEBUG(method, errno_val, fmt, ...) _SF_ERRNO_LOG_TEMPLATE(debug, method, errno_val, fmt, __VA_ARGS__)
#define SF_ERRNO_LOG_INFO(method, errno_val, fmt, ...) _SF_ERRNO_LOG_TEMPLATE(info, method, errno_val, fmt, __VA_ARGS__)
#define SF_ERRNO_LOG_WARN(method, errno_val, fmt, ...) _SF_ERRNO_LOG_TEMPLATE(warn, method, errno_val, fmt, __VA_ARGS__)
#define SF_ERRNO_LOG_ERROR(method, errno_val, fmt, ...) _SF_ERRNO_LOG_TEMPLATE(error, method, errno_val, fmt, __VA_ARGS__)
#define SF_ERRNO_LOG_FATAL(method, errno_val, fmt, ...) _SF_ERRNO_LOG_TEMPLATE(fatal, method, errno_val, fmt, __VA_ARGS__)

#include "global_log.inl"
