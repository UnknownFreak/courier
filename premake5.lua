-- premake5.lua
workspace "courier"
	configurations { "debug", "release" }

	startproject "example"

	warnings "Extra"
	
	cppdialect "C++20"
	language "C++"
	architecture "x64"
	
	flags { "FatalCompileWarnings", "MultiProcessorCompile"}
	
	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"
		
	filter "configurations:release"
		defines {"NDEBUG" }
		optimize "On"

	filter "action:gmake"
		linkoptions { "-fopenmp" }


include "courier.lua"
	
		
project "example"
	kind "consoleApp"
	
	targetdir "bin/%{cfg.buildcfg}"

	includedirs { "include" }

	links { "courier" }
	files { "example/**.cpp" }

