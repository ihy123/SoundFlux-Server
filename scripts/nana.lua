project "nana"
	kind "StaticLib"
	language "C++"
	location "../vendor/nana"
	targetdir "../bin/%{cfg.buildcfg}"
	
	-- set up files
	files { "../vendor/nana/source/**.c", "../vendor/nana/source/**.cpp",
		"../vendor/nana/source/**.h", "../vendor/nana/source/**.hpp" }
	
	includedirs { "../vendor/nana/include" }

	-- set up configurations
	filter "configurations:Release"
		optimize "Speed"
		runtime "Release"
		staticruntime "On"
	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"
	