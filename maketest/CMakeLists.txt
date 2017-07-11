cmake_minimum_required (VERSION 2.6)
project (Hello)

# version number
set (Hello_VERSION_MAJOR 2)
set (Hello_VERSION_MINOR 0)

# reprocessing stuff
configure_file (
  "${PROJECT_SOURCE_DIR}/hello.h.in"
  "${PROJECT_SOURCE_DIR}/hello.h"
  )

# cmake auto-linking policy with qtmain
cmake_policy(SET CMP0020 NEW)

# qt flags
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# dependencies
# search path
include_directories("${PROJECT_BINARY_DIR}")



find_package(OpenSceneGraph 3.0.0 REQUIRED osgDB osgUtil)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

message(OPENSCENEGRAPH_FOUND)
message(OPENSCENEGRAPH_VERSION)
message(OPENSCENEGRAPH_INCLUDE_DIRS )
message(OPENSCENEGRAPH_LIBRARIES )
message(${OPENSCENEGRAPH_INCLUDE_DIRS})
message(${OPENSCENEGRAPH_LIBRARIES})

find_package(Qt5Widgets)
#get_target_property(QtWidgets Qt5::Core LOCATION)

# source
file(GLOB hello_src "*.h" "*.cpp")

# final add executable
add_executable(hello ${hello_src})

# linking
target_link_libraries(hello Qt5::Widgets)
target_link_libraries(foo ${OPENSCENEGRAPH_LIBRARIES})

#install
install(TARGETS hello DESTINATION "bin")
install(DIRECTORY assets DESTINATION .)

# DLL copying
foreach(dll 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    )
  # copy to the Release directory
  add_custom_command(
    TARGET hello 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:${dll}>
      $<TARGET_FILE_DIR:hello>
  )
  # copy to the install directory
  get_target_property(dll_location ${dll} LOCATION)
  install(FILES ${dll_location} DESTINATION "bin")
  message("imported target: ${dll}, path: ${dll_location}")
endforeach(dll)

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("debug mode")
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

# # build a CPack driven installer package
# include (InstallRequiredSystemLibraries)
# set (CPACK_RESOURCE_FILE_LICENSE  
#      "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
# set (CPACK_PACKAGE_VERSION_MAJOR "${Tutorial_VERSION_MAJOR}")
# set (CPACK_PACKAGE_VERSION_MINOR "${Tutorial_VERSION_MINOR}")
# include (CPack)


# CPACK
set(CPACK_GENERATOR WIX)
set(CPACK_PACKAGE_NAME "MyLib")
set(CPACK_PACKAGE_VENDOR "CMake.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MyLib - CPack Component Installation Example")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "VSim")
# set(CPACK_NSIS_MODIFY_PATH ON)

include(CPack)
