# courier
A simple channel based message bus in c++

## Requirements
Premake5

This project uses premake5 to generate the project files (https://premake.github.io/)

### linux
gcc version that supports c++20 or later

### windows
msvc compiler that supports c++20 or later

## building
	The helper scripts assumes that premake5 is added to your system path or equivalent
### windows
	run the premake.bat script and open the generated visual studio solution
	the premake.bat script is equivalent to running premake5 vs2022
	in visual studio build the project and run the example project

### linux
	run the premake.sh script and then make the project.
	the premake.sh script is equivalent to running premake5 gmake
```
	./premake.sh
	make -jX # where X is number of parallel jobs.
	make
	./bin/debug/sample
```

## including in your project

### with your project using premake
	If you're using premake yourself, include the courier.lua in your premake5 project file.

	Example 
	```
	workspace 
		...

	include "path_to_courier_root/courier.lua"

	project "myproject"
	
		includedirs { "path_to_courier_root/include" }
		links { "courier" }
		openmp "On"

	```
	Note that openmp is "on" when building the project that uses the courier library, since the channel templates will fail to build without it


	in order to link properly when making linux applications add the following line to the workspace or to the application project(s)
	```
		filter "action:gmake"
			linkoptions { "-fopenmp" }
	```

### your project using other build system
#### for windows
	it's recommended to generate a vs solution with premake to then manually add it to your solution project
	make sure your project enables the openmp option to properly link when compiling
#### linux
	it's still recommended is to generate a gmake project and then with your build system add the dependencies manually
	you can then build the static library with `make courier config=release`
	with the generated library it can then be linked by adding the include directory and add courier as a lib
	make sure the openmp compiler option is set when compiling to properly link the final executable
