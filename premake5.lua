-- premake5.lua
workspace "courier"
	configurations { "Debug", "Release" }

	startproject "example"

	warnings "Extra"
	
	cppdialect "C++20"
	language "C++"
	architecture "x64"
	
	flags { "FatalCompileWarnings", "MultiProcessorCompile"}
	
	filter "configurations:Debug"
		defines {"DEBUG"}
		symbols "On"
		
	filter "configurations:Release"
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

