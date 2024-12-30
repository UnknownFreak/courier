# courier
A simple channel based message bus in c++

# building

This project uses premake5 to generate the project files (https://premake.github.io/)

Generate project with either premake.bat for windows or premake.sh for linux

## windows
	in visual studio build the project and run the example project

## linux
```
	make -jX # where X is number of parallel jobs.
	make config=release # to build release
	./bin/debug/example
```

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

in order to link properly when making linux applications add the following line to the workspace or to the application project(s)
```
	filter "action:gmake"
		linkoptions { "-fopenmp" }
```

Manually

Generate the project files, and manually add the references to the project.

# Requirements
Premake5

## linux
g++ 13.1 or later for std::format

## windows
vs 2022 or later
