just dump of osx stuff


ok here is our todo list

"i like work where there is list of tasks
i go through the list and get things done"

VSim_bundle
    cmake generates this dummy OSX_BUNDLE, for metadata and stuff
    uses dummy_main.cpp

VSim.app
    cmake custom target, created through a python script
    build_osx.py

bundle todo list
    copy frameworks into place, recursively fix up dependencies
    copy osg plugins, qt plugins?, fix up dependencies
    copy assets, etc, like everything in cmake install() how to do
    replace VSim_bundle with VSim, fix the Info.plist

app fixes
    fix assets path location
        maybe just make a more dynamic global gAssets(), way easier than QGuiApplication::applicationDirectory() or whatever
    set osg plugin path if OSX


## useful shell commands
install_name_tool -change /the/old/path/to/the_library_name.dylib  "@executable_path/../Frameworks/the_library_name.dylib" ./MyProgram.app/Contents/MacOS/MyProgram
otool -L exeordll.dylib
brew --prefix open-scene-graph
brew --prefix qt
brew info --json=v1 open-scene-graph
export DYLD_PRINT_LIBRARIES=1



PPNAME=MyApp
APPBUNDLE=$(APPNAME).app
APPBUNDLECONTENTS=$(APPBUNDLE)/Contents
APPBUNDLEEXE=$(APPBUNDLECONTENTS)/MacOS
APPBUNDLERESOURCES=$(APPBUNDLECONTENTS)/Resources
APPBUNDLEICON=$(APPBUNDLECONTENTS)/Resources
appbundle: macosx/$(APPNAME).icns
    rm -rf $(APPBUNDLE)
    mkdir $(APPBUNDLE)
    mkdir $(APPBUNDLE)/Contents
    mkdir $(APPBUNDLE)/Contents/MacOS
    mkdir $(APPBUNDLE)/Contents/Resources
    cp macosx/Info.plist $(APPBUNDLECONTENTS)/
    cp macosx/PkgInfo $(APPBUNDLECONTENTS)/
    cp macosx/$(APPNAME).icns $(APPBUNDLEICON)/
    cp $(OUTFILE) $(APPBUNDLEEXE)/$(APPNAME)

macosx/$(APPNAME).icns: macosx/$(APPNAME)Icon.png
    rm -rf macosx/$(APPNAME).iconset
    mkdir macosx/$(APPNAME).iconset
    sips -z 16 16     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_16x16.png
    sips -z 32 32     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_16x16@2x.png
    sips -z 32 32     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_32x32.png
    sips -z 64 64     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_32x32@2x.png
    sips -z 128 128   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_128x128.png
    sips -z 256 256   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_128x128@2x.png
    sips -z 256 256   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_256x256.png
    sips -z 512 512   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_256x256@2x.png
    sips -z 512 512   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_512x512.png
    cp macosx/$(APPNAME)Icon.png macosx/$(APPNAME).iconset/icon_512x512@2x.png
    iconutil -c icns -o macosx/$(APPNAME).icns macosx/$(APPNAME).iconset
    rm -r macosx/$(APPNAME).iconset
Info.plist

<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>English</string>
    <key>CFBundleExecutable</key>
    <string>MyApp</string>
    <key>CFBundleGetInfoString</key>
    <string>0.48.2, Copyright 2013 my company</string>
    <key>CFBundleIconFile</key>
    <string>MyApp.icns</string>
    <key>CFBundleIdentifier</key>
    <string>com.mycompany.MyApp</string>
    <key>CFBundleDocumentTypes</key>
    <array>
    </array>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>0.48.2</string>
    <key>CFBundleSignature</key>
    <string>MyAp</string>
    <key>CFBundleVersion</key>
    <string>0.48.2</string>
    <key>NSHumanReadableCopyright</key>
    <string>Copyright 2013 my company.</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.3</string>
</dict>
</plist>