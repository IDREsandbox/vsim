
install_name_tool -change /the/old/path/to/the_library_name.dylib  "@executable_path/../Frameworks/the_library_name.dylib" ./MyProgram.app/Contents/MacOS/MyProgram

otool -L vsim