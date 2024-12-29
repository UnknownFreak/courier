-- premake5.lua
workspace "courier"
	configurations { "Debug", "Release" }
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

project "courier"
	kind "staticlib"

	targetdir "bin/%{cfg.buildcfg}"
	
	files { "include/**.hpp", "src/**.cpp",
		".gitignore", "LICENSE", "README.md", "premake5.lua", "premake.bat", "premake.sh"	}
	
	vpaths { ["other"] = {".gitignore", "LICENSE", "README.md", "premake5.lua", "premake.bat", "premake.sh"} }
	
	includedirs { "include" }
	
		
project "example"
	kind "consoleApp"
	
	targetdir "bin/%{cfg.buildcfg}"
	
	includedirs { "include" }
	
	links { "courier" }
	files { "example/**.cpp" }
	