#include <pch.h>
#include "utilities.h"

#ifdef SF_PLATFORM_WINDOWS
#include <Windows.h>
#include <ShlObj.h>
#include <bcrypt.h>
#elif defined(SF_PLATFORM_UNIX)
#include <sys/stat.h>
#include <dirent.h>
#include <sys/utsname.h>
#endif

std::string get_config_directory() {
#ifdef SF_PLATFORM_WINDOWS

	std::string dir(MAX_PATH + 1, '\0');
	if (SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, dir.data()) == S_OK) {
		size_t len = strlen(dir.c_str());
		dir[len] = '\\';
		dir.resize(len + 1);
	}

#elif defined(SF_PLATFORM_UNIX)

	std::string dir;
	char* home = getenv("XDG_CONFIG_HOME");
	if (home) {
		dir = home;
		if (dir.back() != '/')
			dir.push_back('/');
	}
	else {
		home = getenv("HOME");
		if (home)
			dir = home + dir.back() != '/' ? "/.config/" : ".config/";
	}

	if (!dir.empty()) {
		DIR* d = opendir();
		if (!d)
			mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
		closedir(d);
	}

#endif

	return dir;
}

std::string get_default_user_agent_str() {
#ifdef SF_PLATFORM_WINDOWS

	/* get pc name */
	DWORD pos = 256;
	char buf[256];
	if (!GetComputerNameA(buf, &pos))
		pos = 0;

	/* get Windows version */
	OSVERSIONINFOEXW osinfo{ .dwOSVersionInfoSize = sizeof(osinfo) };
	using pf_RtlGetVersion = NTSTATUS(WINAPI*)(LPOSVERSIONINFOEXW);
	pf_RtlGetVersion f = (pf_RtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
	if (!f || f(&osinfo)) {
		snprintf(buf + pos, sizeof(buf) - pos, " (OS Windows)");
		return buf;
	}
	switch (osinfo.dwMajorVersion) {
	case 6:
		switch (osinfo.dwMinorVersion) {
		case 0:
			if (osinfo.wProductType == VER_NT_WORKSTATION)
				pos += snprintf(buf + pos, sizeof(buf) - pos, " (Windows Vista");
			else
				pos += snprintf(buf + pos, sizeof(buf) - pos, " (Windows Server 2008");
			break;
		case 1:
			if (osinfo.wProductType == VER_NT_WORKSTATION)
				pos += snprintf(buf + pos, sizeof(buf) - pos, " (Windows 7");
			else
				pos += snprintf(buf + pos, sizeof(buf) - pos, " (Windows Server 2008 R2");
			break;
		case 2:
			if (osinfo.wProductType == VER_NT_WORKSTATION)
				pos += snprintf(buf + pos, sizeof(buf) - pos, " (Windows 8");
			else
				pos += snprintf(buf + pos, sizeof(buf) - pos, " (Windows Server 2012");
			break;
		case 3:
			if (osinfo.wProductType == VER_NT_WORKSTATION)
				pos += snprintf(buf + pos, sizeof(buf) - pos, " (Windows 8.1");
			else
				pos += snprintf(buf + pos, sizeof(buf) - pos, " (Windows Server 2012 R2");
			break;
		}
		break;
	case 10:
		if (osinfo.wProductType == VER_NT_WORKSTATION)
			pos += snprintf(buf + pos, sizeof(buf) - pos, " (Windows 10");
		else
			pos += snprintf(buf + pos, sizeof(buf) - pos, " (Windows Server 2016");
		break;
	default:
		snprintf(buf + pos, sizeof(buf) - pos, " (OS Windows)");
		return buf;
	}

	if (osinfo.szCSDVersion && osinfo.szCSDVersion[0] != L'\0')
		snprintf(buf + pos, sizeof(buf) - pos, " %ws)", osinfo.szCSDVersion);
	else
		snprintf(buf + pos, sizeof(buf) - pos, ")");

	return buf;

#elif defined(SF_PLATFORM_UNIX)

	utsname sysinfo;
	if (uname(&sysinfo) == 0)
		return sysinfo.nodename + " (" + sysinfo.sysname + " " + sysinfo.release + ")";

	return "Unix";

#endif

	return "Unnamed";
}
