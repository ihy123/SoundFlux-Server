#pragma once

#ifdef SF_PLATFORM_WINDOWS

#include <WinSock2.h>

std::string _format_wsa_last_error(int wsaerr);

#define _NET_LOG_TEMPLATE(suffix, method, fmt, ...) SF_LOG_##suffix(method, "WinSock2 error: {} [{}] " fmt,\
	WSAGetLastError(), _format_wsa_last_error(WSAGetLastError()), __VA_ARGS__)
#define NET_LOG_DEBUG(method, fmt, ...)  _NET_LOG_TEMPLATE(DEBUG, method, fmt, __VA_ARGS__)
#define NET_LOG_INFO(method, fmt, ...)   _NET_LOG_TEMPLATE(INFO, method, fmt, __VA_ARGS__)
#define NET_LOG_WARN(method, fmt, ...)   _NET_LOG_TEMPLATE(WARN, method, fmt, __VA_ARGS__)
#define NET_LOG_ERROR(method, fmt, ...)  _NET_LOG_TEMPLATE(ERROR, method, fmt, __VA_ARGS__)
#define NET_LOG_FATAL(method, fmt, ...)  _NET_LOG_TEMPLATE(FATAL, method, fmt, __VA_ARGS__)

#elif defined(SF_PLATFORM_UNIX)

#define NET_LOG_DEBUG(method, ...)  SF_ERRNO_LOG_DEBUG(method, errno, __VA_ARGS__)
#define NET_LOG_INFO(method, ...)   SF_ERRNO_LOG_INFO(method, errno, __VA_ARGS__)
#define NET_LOG_WARN(method, ...)   SF_ERRNO_LOG_WARN(method, errno, __VA_ARGS__)
#define NET_LOG_ERROR(method, ...)  SF_ERRNO_LOG_ERROR(method, errno, __VA_ARGS__)
#define NET_LOG_FATAL(method, ...)  SF_ERRNO_LOG_FATAL(method, errno, __VA_ARGS__)

#endif
