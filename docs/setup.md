# Setup

## Windows Simple - Visual Studio 2017, CMake, vcpkg pre-built

- install Visual Studio 2017, make sure to check C++
- install git bash
- install cmake
- download pre-built vcpkg dependencies (ask me for a google drive link)
- extract dependencies

building

```
mkdir build
cd build

TCFILE="C:/Users/David/Desktop/vcpkg-export-20180416-085735/scripts/buildsystems/vcpkg.cmake"
cmake -G "Visual Studio 15 2017 Win64" \
  -DCMAKE_TOOLCHAIN_FILE=$TCFILE \
  -DCMAKE_INSTALL_PREFIX=install ..
```

installing, you might have to run install twice for all dll copying to work

```
cmake --build . --target INSTALL --config RELEASE
```

## Mac OSX

- install git
- install homebrew
- `brew install cmake`

```
brew install flatbuffers open-scene-graph qt cryptopp assimp python3

mkdir build
cd build

dprefix=""
for dep in "qt"; do dprefix+=$(brew --prefix $dep)";"; done

cmake -DCMAKE_PREFIX_PATH=$dprefix \
  -DCMAKE_INSTALL_PREFIX=install ..
```


# old setup instructions that probably don't work


## Windows - Visual Studio 2017, Qt5, CMake, vcpkg build dependencies

You can either (1) use CMake to generate Visual Studio project files or (2) use Visual Studio alone with a Qt extension. CMake is used for the release builds.

Essentials
- install Visual Studio 2017, the free one
- install git bash
- `git clone https://github.com/IDREsandbox/vsim.git`

### Install Visual Studio 2017

- just install it
1. `git clone https://github.com/IDREsandbox/vsim.git`

### CMake without vcpkg

You have to install qt, osg, and osg third party binaries.

```
mkdir build
cd build

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

### Setup Visual Studio 2017, no CMake

1. `git clone https://github.com/IDREsandbox/vsim.git`, the solution is in src/windows
2. PATH - right click vsim project in the Solution Explorer (or click Project in the top tool bar) > Properties > Debugging > Environment
	`PATH=$(QTDIR)\bin;$(DEPS)\3rdParty-build\bin;$(DEPS)\osg-build\install\bin;%PATH%`
3. Working Directory, this is for the little icon and fonts - `$(ROOT)`
4. Dependencies - setup a symlink to the dependencies. The project assumes that dependencies are located in the root git directory, so you have to give it a link to the actual dependencies. If you're at the sandbox, dependencies are in vsim/vsim-dependencies.
	`mklink /D dependencies T:\Projects\_UCLA\vsim\vsim-dependencies`
	In powershell:
	`cmd /c mklink /D dependencies T:\Projects\_UCLA\vsim\vsim-dependencies`
5. Qt Version - right click vsim project in the Solution Explorer > Qt Project Settings > Version > link to the corresponding version (have to do this for 32 and 64 if you have both).


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
