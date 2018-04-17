# Setup

## Windows - Visual Studio 2017, Qt5, CMake, vcpkg

You can either (1) use CMake to generate Visual Studio project files or (2) use Visual Studio alone with a Qt extension. CMake is used for the release builds. I eventually started using CMake only because the Qt extension was slow, so the installation instructions are probaly out of date.

Essentials
- install Visual Studio 2017, the free one
- install git bash
- `git clone https://github.com/IDREsandbox/vsim.git`

### Setup Qt

1. Download Qt Open Source https://www.qt.io/download-open-source/
2. Install Qt 5.10.x 64bit VS2017. Notes <5.9 has bugs with thumbnails, 5.10.1 has graphical bugs
3. In Visual Studio -
	- Install QT Visual Studio Tools (Tools > Extensions and Updates, OR https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools-19123)
	- QT5 > QtOptions - add the Qt versions.
  - This isn't necessary for CMake but it's convenient. I use it for test projects.

### Install Visual Studio 2017

- just install it
1. `git clone https://github.com/IDREsandbox/vsim.git`

### vcpkg

So dependency management used to be a big pain. I had to figure out how to compile osg and its dependencies then put the dlls and stuff in a thumb drive... The new thing is vcpkg.

TODO: export everything in a vcpkg zip so you don't have to rebuild.

Download vcpkg https://github.com/Microsoft/vcpkg

```
./bootstrap-vcpkg.bat
./vcpkg integrate install
```

#### Flatbuffers

```
./vcpkg install flatbuffers:x64-windows
```

You have to add the flatbuffer compiler flatc to path.
It's easiest just to add vcpkg/installed/x64-windows/tools to path.

#### Qt, OpenSceneGraph

For osg you can copy my folder. You don't really need to do qt if you use the qt installer. These build trees are kind of enormous - up to 40GB, and can take several hours to build. You can delete the big buildtrees folder afterwards.

```
./vcpkg install osg:x64-windows
./vcpkg install qt5-base:x64-windows qt5-svg:x64-windows qt5-imageformats:x64-windows
```

### CMake (option 1)

Download CMake and add it to PATH https://cmake.org/download/
CMake is used to build releases from the command prompt.

```
mkdir build
cd build
```

```
cmake -G "Visual Studio 15 2017 Win64" \
  -DCMAKE_TOOLCHAIN_FILE=C:/Users/David/Desktop/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_INSTALL_PREFIX=install ..
```

The old way
```
cmake -G "Visual Studio 15 2017 Win64" \
  -DCMAKE_TOOLCHAIN_FILE=C:/Users/David/Desktop/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DQT_DIR="${QT_DIR}" \
  -DOSG_DIR="${OSG_DIR}" \
  -DTP_BIN="${TP_BIN}" \
  -DCMAKE_INSTALL_PREFIX=install ..
```

```
cmake --build . --target INSTALL --config Release
```

### Setup Visual Studio 2017 (option 2)

1. `git clone https://github.com/IDREsandbox/vsim.git`, the solution is in src/windows
2. PATH - right click vsim project in the Solution Explorer (or click Project in the top tool bar) > Properties > Debugging > Environment
	`PATH=$(QTDIR)\bin;$(DEPS)\3rdParty-build\bin;$(DEPS)\osg-build\install\bin;%PATH%`
3. Working Directory, this is for the little icon and fonts - `$(ROOT)`
4. Dependencies - setup a symlink to the dependencies. The project assumes that dependencies are located in the root git directory, so you have to give it a link to the actual dependencies. If you're at the sandbox, dependencies are in vsim/vsim-dependencies.
	`mklink /D dependencies T:\Projects\_UCLA\vsim\vsim-dependencies`
	In powershell:
	`cmd /c mklink /D dependencies T:\Projects\_UCLA\vsim\vsim-dependencies`
5. Qt Version - right click vsim project in the Solution Explorer > Qt Project Settings > Version > link to the corresponding version (have to do this for 32 and 64 if you have both).


### CMake inside Visual Studio 2017 (doesn't work)

VS2017 is supposed to have cmake support. I couldn't figure it out.

1. Open the vsim folder through VS.
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