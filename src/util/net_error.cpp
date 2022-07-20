#include <pch.h>

#ifdef SF_PLATFORM_WINDOWS

#include "net_error.h"
#include <codecvt>

std::string _format_wsa_last_error(int wsaerr) {
	/* retrieve error description as UTF-16 string */
	wchar_t* msg = nullptr;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr, wsaerr, 0, reinterpret_cast<LPWSTR>(&msg), 0u, nullptr);
	if (msg) {
		/* convert to UTF-8 */
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
		std::string result = cvt.to_bytes(msg);
		/* free buffer allocated by system */
		LocalFree(msg);
		return result;
	}
	return "";
}

#endif
