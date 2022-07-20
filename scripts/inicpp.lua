project "inicpp"
	kind "StaticLib"
	language "C++"
	location "../vendor/inicpp"
	targetdir "../bin/%{cfg.buildcfg}"
	
	-- set up files
	files { "../vendor/inicpp/src/*.cpp" }
	
	includedirs { "../vendor/inicpp/include/inicpp" }
	
	-- set up configurations
	filter "configurations:Release"
		optimize "Speed"
		runtime "Release"
		staticruntime "On"
	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"
	