# courier
A simple channel based message bus in c++

# building

This project uses premake5 to generate the project files (https://premake.github.io/)

Generate project with either premake.bat for windows or premake.sh for linux

# including in your project

If you're using premake yourself, include the courier.lua in your premake5.lua file.

Example 
```
workspace 
	...

include "path_to_courier_root/courier.lua"

project "myproject"
	
	includedirs { "path_to_courier_root/include" }
	links { "courier" }

```

Manually

Generate the project files, and manually add the references to the project.