-- premake5.lua
workspace "courier"
	configurations { "debug", "release" }

	startproject "sample"

	warnings "Extra"
	
	cppdialect "C++23"
	language "C++"
	architecture "x64"
	
	flags {"MultiProcessorCompile"}
	
	fatalwarnings { "All" }

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"
		
	filter "configurations:release"
		defines {"NDEBUG" }
		optimize "On"

	filter "action:vs20*"
		fatalwarnings { "5204" }

	filter "action:gmake"
		enablewarnings { "non-virtual-dtor" }
		buildoptions { "-march=native", "-mtune=native"}
		linkoptions { "-fopenmp" }

include "courier.lua"
	
		
project "sample"
	kind "consoleApp"
	
	targetdir "bin/%{cfg.buildcfg}"

	includedirs { "include", "sample", "vnd/lockfree_mpmc_queue" }
	defines {"COURIER_ALLOW_EMPTY_HANDLER=1", "COURIER_LOG_EMPTY_HANDLER=1"}

	links { "courier" }
	files { "sample/**.cpp", "sample/**.hpp" }
	openmp "On"
--[[
project "imcourier"
	kind "staticlib"
	targetdir "bin/%{cfg.buildcfg}"

	includedirs { "include", "extension", "vnd/imgui" }
	defines {"COURIER_ALLOW_EMPTY_HANDLER=1", "COURIER_LOG_EMPTY_HANDLER=1"}

	links { "courier"}
	files { "extension/*.cpp"}
	openmp "On"

project "gruntime"
	kind "consoleApp"
	targetdir "bin/%{cfg.buildcfg}"

	includedirs { "include", "extension", "vnd/imgui" }
	defines {"COURIER_ALLOW_EMPTY_HANDLER=1", "COURIER_LOG_EMPTY_HANDLER=1"}

	links { "courier", "imcourier", "SDL3" }
	files { "runtime/*.cpp", "vnd/imgui/*.cpp", "vnd/imgui/backends/imgui_impl_sdlrenderer3.cpp", "vnd/imgui/backends/imgui_impl_sdl3.cpp"}
	openmp "On"
--]]
project "tests"
	kind "consoleApp"
	targetdir "bin/%{cfg.buildcfg}"

	includedirs { "include", "tests", "vnd/lockfree_mpmc_queue"}
	defines {"COURIER_ALLOW_EMPTY_HANDLER=1", "COURIER_LOG_EMPTY_HANDLER=1"}

	links { "courier", "Catch2", "pthread"}
	files { "tests/*.cpp", "tests/*.hpp" }
	openmp "On"
