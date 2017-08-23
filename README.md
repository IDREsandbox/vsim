
# VSim

## Installation

### Windows - CMake, Visual Studio 2017, Qt 5.9.1

Downloads

 - Qt Open Source https://www.qt.io/download-open-source/, Install Qt 5.9.1 64 bit VS2017
 - CMake, add to path
 - Visual Studio 2017

Building (For more cmake notes see cmake.txt)

 1. In the vsim root
 	```
	mkdir build
	cd build
	cmake -G "Visual Studio 15 2017 Win64" \
	  -DCMAKE_PREFIX_PATH="C:/Qt/5.9.1/msvc2017_64;T:/Projects/_UCLA/vsim/vsim-dependencies/OpenSceneGraph-3.4.0-build/msvc2017_64/install" \
	  -DOPENSCENEGRAPH_BINARY_DIR="T:/Projects/_UCLA/vsim/vsim-dependencies/OpenSceneGraph-3.4.0-build/msvc2017_64/install/bin" \
	  -DTHIRDPARTY_BINARY_DIR="T:/Projects/_UCLA/vsim/vsim-dependencies/3rdParty-build/msvc2017_64/3rdParty/-x64/bin" \
	  -DQT_DIR="C:/Qt/5.9.1/msvc2017_64/" \
	  -DCMAKE_INSTALL_PREFIX=install ..
	  ```
 2. To build (use Debug for debug)
	`cmake --build . --config Release`
 3. To install
	`cmake --build . --target INSTALL --config Release`
 4. To package (into installer)
	`cpack .`
 6. Running tests `cmake -E env CTEST_OUTPUT_ON_FAILURE=1 cmake --build . --config RELEASE --target RUN_TESTS`
 7. If you add new files or make big changes `touch CMakeLists.txt` refreshes the source list

CMake inside Visual Studio 2017 (doesn't fully work yet)

 1. VS2017 has some cmake support. Open the vsim folder through VS.
 2. Poke the CMakeLists.txt, it should try to run cmake and fail
 3. Right click CMakeList.txt > Change CMake Settings
 4. paste this at the end of "x64-Debug" and "x64-Release"
     ````
        "variables": [
          {
            "name": "CMAKE_PREFIX_PATH",
            "value": "C:/Qt/5.9.1/msvc2017_64;T:/Projects/_UCLA/vsim/vsim-dependencies/OpenSceneGraph-3.4.0-build/msvc2017_64/install"
          },
          {
            "name": "OPENSCENEGRAPH_BINARY_DIR",
            "value": "T:/Projects/_UCLA/vsim/vsim-dependencies/OpenSceneGraph-3.4.0-build/msvc2017_64/install/bin"
          },
          {
            "name": "THIRDPARTY_BINARY_DIR",
            "value": "T:/Projects/_UCLA/vsim/vsim-dependencies/3rdParty-build/msvc2017_64/3rdParty/-x64/bin"
          },
          {
            "name": "QT_DIR",
            "value": "C:/Qt/5.9.1/msvc2017_64/plugins/platforms"
          }
        ]
    ````
 5. Right click CMakeLists.txt > build VSim.exe
 6. Right click CMakeLists.txt > Debug and Launch Settings. Paste this
     ````
	     "currentDir": "${workspaceRoot}",
         "env": "PATH=C:/Qt/5.9.1/msvc2017_64/bin/;T:/Projects/_UCLA/vsim/vsim-dependencies/OpenSceneGraph-3.4.0-build/msvc2017_64/install/bin;T:/Projects/_UCLA/vsim/vsim-dependencies/OpenSceneGraph-3.4.0-build/msvc2017_64/install/bin/osgPlugins-3.4.0/;T:/Projects/_UCLA/vsim/vsim-dependencies/3rdParty-build/msvc2017_64/3rdParty/-x64/bin"
	 ````

### OSX

 1. `brew install qt5 openscenegraph`
 2. `git clone https://github.com/IDREsandbox/vsim.git`
 3. in the vsim repo
	```
	mkdir build
	cd build
	cmake \
	  -DCMAKE_PREFIX_PATH="/usr/local/Cellar/qt/5.9.1" \
	  -DCMAKE_INSTALL_PREFIX=install ..
	```
 4. Building and installing `cmake --build . --config RELEASE --target install`
 5. Packaging `cpack .`

### Windows - Visual Studio 2015

Setup Qt

1. Download Qt Open Source https://www.qt.io/download-open-source/
2. Install Qt 5.8 64bit VS2015.
3. In Visual Studio - 
	- Install QTPackage plugin (Tools > Extensions and Updates, OR https://marketplace.visualstudio.com/items?itemName=havendv.QtPackage)
	- QT5 > QtOptions - add the Qt versions. Name the x64 version 5.8.

Setup Visual Studio 2015

1. `git clone https://github.com/IDREsandbox/vsim.git`, the 2015 solution is in src/windows
2. PATH - right click vsim project in the Solution Explorer (or click Project in the top tool bar) > Properties > Debugging > Environment 
	`PATH=$(QTDIR)\bin;$(DEPS)\3rdParty-build\msvc2015_$(PlatformArchitecture)\bin;$(DEPS)\OpenSceneGraph-3.4.0-build\msvc2015_$(PlatformArchitecture)\bin;%PATH%`
3. Working Directory, this is for the little icon and fonts - `$(ROOT)`
4. Dependencies - setup a symlink to the dependencies. The project assumes that dependencies are located in the root git directory, so you have to give it a link to the actual dependencies. If you're at the sandbox, dependencies are in vsim/vsim-dependencies.
	`mklink /D dependencies T:\Projects\_UCLA\vsim\vsim-dependencies`
5. Qt Version - right click vsim project in the Solution Explorer > Qt Project Settings > Version > link to the corresponding version (have to do this for 32 and 64 if you have both). 





## Style guide

- tabs
- function brackets, doesn't really matter
	```
	void foo()
	{
		bar();
	}
- if/else
	```
	if (something) {
		foo();
	} 
	else if (something_else) {
		bar();
	} 
	else {
		zzz();
	}
- \\n line endings
- include guards (no #pragma once)
- sensible #includes, the old code makes a mess of #include
- .cpp, .h, MyClass.cpp
- int m_member_variable; class MyClass; typedef SomeType; MyClass::memberFunction
- avoid exceptions, do simple error handling - lots of if/else, enum error codes, NULLs, etc
- avoid crazy templates and crazy macros
- logging? do we even log? we should add a log. For now use qDebug(), qInfo(), etc
- smart pointers: QObjects just use raw pointers, osg stuff use refs or raw pointers
