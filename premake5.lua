workspace "SoundFlux"
	configurations { "Debug", "Release" }
	platforms { "x86_64" }
	cppdialect "C++20"
	
project "SoundFlux-Server"
	kind "WindowedApp"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	
	-- set up files
	files { "src/**.c", "src/**.cpp", "src/**.h", "src/**.hpp", "src/**.inl", "pch/pch.h", "pch/pch.cpp" }
	
	-- set up precompiled header
	pchheader "pch.h"
	pchsource "pch/pch.cpp"
	includedirs { "pch" }
	
	-- build dependencies
	include "vendor/nana"
	
	-- link dependencies
	links { "nana" }
	filter "system: windows"
		links { "Ws2_32" }

	-- set up configurations
	filter "configurations:Release"
		optimize "Speed"
		runtime "Release"
	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"
	
	-- set up platforms
	filter "platforms:x86"
        architecture "x86"
    filter "platforms:x86_64"
        architecture "x86_64"
