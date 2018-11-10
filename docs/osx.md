
This is mostly about building on mac.
For development I just used VSCode and cmake. I couldn't get XCode working nicely.

## VSim.app

This is called a Bundle, an easy way to distribute apps on osx.
Bundles follow a specific format

VSim.app/
    Contents/
        Info.plist
        MacOS/
            VSim
        Resources/
            vsim.icns
            assets/
                default.vsim
        Frameworks/
            QtCore.dylib
        Plugins/
            imageformats/
                jpg.dylib
            osgPlugins-3.6.2/
                osgdb_obj.so

Info.plist specifies application metadata.
Frameworks are supposed to have more stuff like include, versions, etc, but eh.

## cmake stuff

VSim.app (target)
    cmake custom target, created through a python script
    runs build_osx.py to create VSim.app

BundleCopyExe (target)
    assumes VSim.app is already set up, simply copies over the exe
    this exists so that you can test without re-run the slow build script every time you test

## osx

Info.plist
    template plist

deps.py
    code which builds VSim.app
    copies VSim
    find and copies qt plugins, osg plugins
    recursively finds and copies qt, osg, and other 3rd party libs
    fixes VSim, plugins, and libs to point only to the copied versions
    copies assets

build_osx.py
    command line interface wrapper around deps.py
    'build' for building
    'update' for updating the exe
    everything else is just used for debugging

bundle todo list
    copy frameworks into place, recursively fix up dependencies
    copy osg plugins, qt plugins?, fix up dependencies
    copy assets, etc, like everything in cmake install() how to do
    replace VSim_bundle with VSim, fix the Info.plist

qt.conf
    points qt plugins to Plugins rather that qt/plugins
    points qt binaries/libraries to Frameworks rather that qt/bin
    I don't know if this is necessary



## useful shell commands

install_name_tool -change /the/old/path/to/the_library_name.dylib  "@executable_path/../Frameworks/the_library_name.dylib" ./MyProgram.app/Contents/MacOS/MyProgram
otool -L exeordll.dylib
brew --prefix open-scene-graph
brew --prefix qt
brew info --json=v1 open-scene-graph
export DYLD_PRINT_LIBRARIES=1

## other

If the first person camera if flying all over the place, it's because the app doesn't have permission to center the mouse. Give VSim and maybe Terminal accessibility permission.