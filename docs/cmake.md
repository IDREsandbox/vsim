# CMake Cheat Sheet

## How to Build VSim

```
	mkdir build
	cd build

	QTDIR="C:/Qt/5.9.2/msvc2017_64"
	OSGDIR="C:/Program Files/OpenSceneGraph"
	TPBIN=C:/Users/David/Desktop/vsim/dependencies/3rdParty-build/x64/bin

	cmake -G "Visual Studio 15 2017 Win64" \
	  -DQT_DIR="${QTDIR}" \
	  -DOSG_DIR="${OSGDIR}" \
	  -DTP_BIN="${TPBIN}" \
	  -DCMAKE_INSTALL_PREFIX=install ..

	cmake --build . --target INSTALL --config Release
```

Making the visual studio project (starting in the root)

mkdir build
cd build
cmake ..

Other generators
cmake -G "Visual Studio 15 2017 Win64" ..

Special install directory, for testing installation folder.
The default on windows is Program Files so for testing don't mess with that
cmake -D CMAKE_INSTALL_PREFIX=install ..

Building (starting in the build directory)
cmake --build .

Building release
cmake --build . --config Release

Installing (equivalent of 'make install' on Linux)
cmake --build . --target INSTALL --config Release

Packaging, default generator is WIX
cmake --build . --config Release
cpack .


--------------

find_package uses CMAKE_PREFIX_PATH to find libs

QT_INSTALL_DIR="C:/Qt/5.9.2/msvc2017_64"
OSG_INSTALL_DIR="C:/Program Files/OpenSceneGraph"

cmake -D CMAKE_PREFIX_PATH=${QT_INSTALL_DIR};${OSG_INSTALL_DIR}

C:/Program Files/OpenSceneGraph


##With Visual Studio 2017, this thing is so strange

Install Qt 5.9.1,

Open Visual Studio 2017, open the vsim folder
click on CMakeLists.txt, save (this will cause Visual Studio to try to run cmake)

right click CMakeLists.txt > Change CMake Settings
CMakeLists.txt should pop up

  -DCMAKE_PREFIX_PATH=C:/Qt/5.9.1/msvc2017_64;T:/Projects/_UCLA/vsim/vsim-dependencies/OpenSceneGraph-3.4.0-build/msvc2017_64/install
  -DOPENSCENEGRAPH_BINARY_DIR=T:/Projects/_UCLA/vsim/vsim-dependencies/OpenSceneGraph-3.4.0-build/msvc2017_64/install/bin
  -DTHIRDPARTY_BINARY_DIR=T:/Projects/_UCLA/vsim/vsim-dependencies/3rdParty-build/msvc2017_64/3rdParty/-x64/bin

paste this at the end of "x64-Debug" and "x64-Release"
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
          }
        ]

Make sure the VS Configuration is on x64-Debug/Release.

right click CMakeLists.txt > build VSim.exe
wait a long time


## Testing

[testing](testing.md)


## Dependencies

[dependencies](dependencies.md)

## Objects

To make tests more fine grained we split the project up into object libraries. Ideally these would correspond to different folders but we haven't gotten around to it yet.

The benefit is that to test Group you don't need to recompile the entire project.

Util_objects - stuff used by everything, util functions, GroupTemplate
Deprecated_objects - the old stuff
ERModel_objects - gui independent ER stuff, depends on Util, Deprecated
NarrativeModel_objects - gui independent Narrative stuff, depends on Util, Deprecated
VSim_objects - everything else
VSim - main executable

# Writing CMake

## How to copy a folder

ex: assets

Copy a folder into working directory /build.

`file(COPY assets DESTINATION "${CMAKE_BINARY_DIR}")`

Copy a folder into executable directory. add_custom_target is run on every
recompile, so to prevent re-copying files all the time you use
add_custom_command, then make the target a dependency.

```
add_custom_command(
  OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/assets
  COMMAND echo "Copying assets"
  COMMAND COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/assets)
add_custom_target(CopyAssets
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/assets)
```

Copy a folder into install directory.

`install(DIRECTORY assets DESTINATION .)`

## Styling

Everything is case insensitive in cmake.

variableNames
TargetNames
TargetName_src
TargetName_test
Library_obj