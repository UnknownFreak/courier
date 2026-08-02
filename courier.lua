
result, errorCode = os.outputof("git describe --tags --always --dirty=-d")
s = string.format(
[[
#include <courier/courier.hpp>
#include <string_view>

namespace courier
{
    const std::string_view getVersion()
    {
        return "%s";
    }
}
]], result)
io.writefile("src/version.cpp", s)

project "courier"
	kind "staticlib"

	targetdir "bin/%{cfg.buildcfg}"
	
	files { "include/**.hpp", "src/**.cpp",
		".gitignore", "LICENSE", "README.md", "premake5.lua", "premake.bat", "premake.sh", "courier.lua"}
	
	vpaths { ["other"] = {".gitignore", "LICENSE", "README.md", "premake5.lua", "courier.lua", "premake.bat", "premake.sh"} }
	
	includedirs { "include" }

	openmp "On"
