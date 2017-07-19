
# VSim

## Installation

### Windows - Visual Studio 2015

Setup Qt

1. Download Qt Open Source https://www.qt.io/download-open-source/
2. Install Qt 5.8 64bit VS2015. For the latest CMake build install 5.9.1 VS2017.
3. In Visual Studio - 
	- Install QTPackage plugin (Tools > Extensions and Updates, OR https://marketplace.visualstudio.com/items?itemName=havendv.QtPackage)
	- QT5 > QtOptions - add the Qt versions. Name the x64 version 5.8.

Setup CMake, Qt 5.9.1, Visual Studio 2017

1. In the vsim root
	mkdir build
	cd build
	cmake -G "Visual Studio 15 2017 Win64" \
	  -D CMAKE_PREFIX_PATH="C:/Qt/5.9.1/msvc2017_64;T:/Projects/_UCLA/vsim/vsim-dependencies/OpenSceneGraph-3.4.0-build/msvc2017_64/install" \
	  -D OPENSCENEGRAPH_BINARY_DIR="T:/Projects/_UCLA/vsim/vsim-dependencies/OpenSceneGraph-3.4.0-build/msvc2017_64/install/bin" \
	  -D THIRDPARTY_BINARY_DIR="T:/Projects/_UCLA/vsim/vsim-dependencies/3rdParty-build/msvc2017_64/3rdParty/-x64/bin" \
	  -D CMAKE_INSTALL_PREFIX=install ..
2. To build (use Debug for debug)
	cmake --build . --config Release
3. To install
	cmake --build . --target INSTALL --config Release
4. To package (into installer)
	cpack .
5. For more cmake instructions see cmake.txt

CMake inside Visual Studio 2017



Setup Visual Studio 2015

1. `git clone https://github.com/IDREsandbox/vsim.git`, the 2015 solution is in src/windows
2. PATH - right click vsim project in the Solution Explorer (or click Project in the top tool bar) > Properties > Debugging > Environment 
	`PATH=$(QTDIR)\bin;$(DEPS)\3rdParty-build\msvc2015_$(PlatformArchitecture)\bin;$(DEPS)\OpenSceneGraph-3.4.0-build\msvc2015_$(PlatformArchitecture)\bin;%PATH%`
3. Working Directory, this is for the little icon and fonts - `$(ROOT)/src`
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
