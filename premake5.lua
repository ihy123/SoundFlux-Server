workspace "SoundFlux"
	configurations { "Debug", "Release" }
	architecture "x86_64"
	cppdialect "C++20"
		
include "scripts/nana.lua"
include "scripts/ssdp-connect.lua"
include "scripts/inicpp.lua"

project "SoundFlux-Server"
	kind "ConsoleApp"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	
	-- set up files
	files { "src/**.c", "src/**.cpp", "src/**.h", "src/**.hpp", "src/**.inl", "pch/pch.h", "pch/pch.cpp" }
	
	-- set up precompiled header
	pchheader "pch.h"
	pchsource "pch/pch.cpp"
	
	-- set include directories
	includedirs { "pch", "vendor/nana/include", "vendor/ssdp-connect", "vendor/spdlog/include", "vendor/inicpp/include" }
		
	-- link dependencies
	links { "nana", "ssdp-connect", "inicpp" }

	filter "system:windows"
		defines { "SF_PLATFORM_WINDOWS" }
		links { "Ws2_32.lib" }

	filter "system:linux or android"
		defines { "SF_PLATFORM_UNIX" }
		links { "pthread", "dl" }

	-- set up configurations
	filter "configurations:Release"
		optimize "Speed"
		runtime "Release"
		staticruntime "On"
	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"
	