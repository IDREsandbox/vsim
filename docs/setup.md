# Setup

## Windows - Visual Studio 2017, CMake, vcpkg

- install Visual Studio 2017, make sure to check C++
- install git bash
- install cmake
- download pre-built vcpkg dependencies (ask me for a onedrive link) or build dependencies [building dependencies](docs/dependencies.md)
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

installing (into build/install)

```
cmake --build . --target INSTALL --config RELEASE
```

(optional)

- install qt, qt visual studio addon : this is useful for experimentation

## Mac OSX

- install git
- install homebrew
- `brew install cmake`
- `brew install flatbuffers open-scene-graph qt cryptopp assimp python3`

```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=install ..
```

Build the executable

```
cmake --build . --target VSim --config Release
```

Package everything into an osx bundle

```
cmake --build . --target VSim.app --config Release
```

Make a tarball.

```
tar czf VSim.tar.gz VSim.app
```

You can do `-G Xcode` for an Xcode project. The default is just a makefile.
I didn't have much success working with Xcode.
You might need to add this? I put in a qt install symlink (/usr/local/opt/qt) so I didn't need it.
I haven't tried the osx build on other machines... so this might not actually work.

```
dprefix=""
for dep in "qt"; do dprefix+=$(brew --prefix $dep)";"; done
-DCMAKE_PREFIX_PATH=$dprefix
```

# old setup instructions that probably don't work

(removed)

other things done/tried

- Visual Studio solution. We used to use this, pain to manage dependencies, hard to source control, I still use this for my experimental code
- Building osg, building osg third party libs. We used to do this, it's a pain and takes all day, vcpkg is a blessing
- Visual Studio 2017 cmake support CMakeSettings.json, didn't work
- CMake + Xcode, couldn't get archiving/bundling to work