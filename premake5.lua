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


result, errorCode = os.outputof("git describe --tags --always --dirty=-d")
s = string.format(
[[
#include <courier/courier.hpp>
#include <string>

namespace courier
{
    const std::string getVersion()
    {
        return "%s";
    }
}
]], result)
io.writefile("src/version.cpp", s)

include "courier.lua"
	
		
project "example"
	kind "consoleApp"
	
	targetdir "bin/%{cfg.buildcfg}"

	includedirs { "include" }

	links { "courier" }
	files { "example/**.cpp" }

