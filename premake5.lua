workspace "SoundFlux"
	configurations { "Debug", "Release" }
	architecture "x86_64"
	--platforms { "x86_64" }
	cppdialect "C++20"
		
include "vendor/nana"

project "SoundFlux-Server"
	kind "ConsoleApp"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	
	-- set up files
	files { "src/**.c", "src/**.cpp", "src/**.h", "src/**.hpp", "src/**.inl", "pch/pch.h", "pch/pch.cpp" }
	
	-- set up precompiled header
	pchheader "pch.h"
	pchsource "pch/pch.cpp"
	
	includedirs { "pch", "vendor/nana/include" }
		
	-- link dependencies
	links { "nana", "Ws2_32.lib" }
		
	-- set up configurations
	filter "configurations:Release"
		optimize "Speed"
		runtime "Release"
		staticruntime "On"
	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"
	