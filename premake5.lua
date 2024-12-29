-- premake5.lua
workspace "courier"
	configurations { "Debug", "Release" }

	startproject "example"

	warnings "Extra"
	flags { "FatalWarnings", "MultiProcessorCompile" }
	
	cppdialect "C++20"
	language "C++"
	architecture "x64"
	
	filter "configurations:Debug"
		defines {"DEBUG"}
		symbols "On"
		
	filter "configurations:Release"
		defines {"NDEBUG" }
		optimize "On"


include "courier.lua"
	
		
project "example"
	kind "consoleApp"
	
	targetdir "bin/%{cfg.buildcfg}"
	
	includedirs { "include" }
	
	links { "courier" }
	files { "example/**.cpp" }
	