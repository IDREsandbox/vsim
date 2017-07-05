
# VSim

## Installation

### Windows - Visual Studio 2015

Setup Qt

1. Download Qt Open Source https://www.qt.io/download-open-source/
2. Install Qt 5.8, 32 and/or 64 bit versions
3. In Visual Studio - 
	- Install QTPackage plugin (Tools > Extensions and Updates, OR https://marketplace.visualstudio.com/items?itemName=havendv.QtPackage)
	- QT5 > QtOptions - add the Qt versions. Name the x64 version 5.8.


Setup Visual Studio

1. `git clone https://github.com/IDREsandbox/vsim.git`, the solution is in src/windows
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
