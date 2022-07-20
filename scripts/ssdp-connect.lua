project "ssdp-connect"
	kind "StaticLib"
	language "C"
	location "../vendor/ssdp-connect"
	targetdir "../bin/%{cfg.buildcfg}"
	
	-- set up files
	files { "../vendor/ssdp-connect/ssdp.h", "../vendor/ssdp-connect/ssdp.c",
		"../vendor/ssdp-connect/ssdp-connect.h", "../vendor/ssdp-connect/ssdp-connect.c" }
	
	-- set up configurations
	filter "configurations:Release"
		optimize "Speed"
		runtime "Release"
		staticruntime "On"
	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"
	