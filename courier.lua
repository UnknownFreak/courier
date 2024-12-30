
project "courier"
	kind "staticlib"

	targetdir "bin/%{cfg.buildcfg}"
	
	files { "include/**.hpp", "src/**.cpp",
		".gitignore", "LICENSE", "README.md", "premake5.lua", "premake.bat", "premake.sh", "courier.lua"}
	
	vpaths { ["other"] = {".gitignore", "LICENSE", "README.md", "premake5.lua", "courier.lua", "premake.bat", "premake.sh"} }
	
	includedirs { "include" }

	openmp "On"
