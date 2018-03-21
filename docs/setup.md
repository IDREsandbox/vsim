# Setup

## Windows - Visual Studio 2017, Qt5, CMake

### Setup Qt

1. Download Qt Open Source https://www.qt.io/download-open-source/
2. Install Qt 5.9.x 64bit VS2017.
3. In Visual Studio -
	- Install QT Visual Studio Tools (Tools > Extensions and Updates, OR https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools-19123)
	- QT5 > QtOptions - add the Qt versions.

### Setup Visual Studio 2017

1. `git clone https://github.com/IDREsandbox/vsim.git`, the solution is in src/windows
2. PATH - right click vsim project in the Solution Explorer (or click Project in the top tool bar) > Properties > Debugging > Environment
	`PATH=$(QTDIR)\bin;$(DEPS)\3rdParty-build\bin;$(DEPS)\osg-build\install\bin;%PATH%`
3. Working Directory, this is for the little icon and fonts - `$(ROOT)`
4. Dependencies - setup a symlink to the dependencies. The project assumes that dependencies are located in the root git directory, so you have to give it a link to the actual dependencies. If you're at the sandbox, dependencies are in vsim/vsim-dependencies.
	`mklink /D dependencies T:\Projects\_UCLA\vsim\vsim-dependencies`
	In powershell:
	`cmd /c mklink /D dependencies T:\Projects\_UCLA\vsim\vsim-dependencies`
5. Qt Version - right click vsim project in the Solution Explorer > Qt Project Settings > Version > link to the corresponding version (have to do this for 32 and 64 if you have both).

### CMake (optional)

Download CMake and add it to PATH https://cmake.org/download/
CMake is used to build releases from the command prompt.

```
mkdir build
cd build

QT_DIR="C:/Qt/5.9.2/msvc2017_64"
OSG_DIR="C:/Program Files/OpenSceneGraph"
TP_BIN=C:/Users/David/Desktop/vsim/dependencies/3rdParty-build/x64/bin

cmake -G "Visual Studio 15 2017 Win64" \
  -DQT_DIR="${QTDIR}" \
  -DOSG_DIR="${OSGDIR}" \
  -DTP_BIN="${TPBIN}" \
  -DCMAKE_INSTALL_PREFIX=install ..

cmake --build . --target INSTALL --config Release
```

If you add new files or make big changes `touch CMakeLists.txt` refreshes the source list

## CMake inside Visual Studio 2017 (doesn't work yet, don't try this)

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

## OSX

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
