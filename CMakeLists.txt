cmake_minimum_required (VERSION 2.8.11)
project (VSim)

# version number
set (VSim_VERSION_MAJOR 2)
set (VSim_VERSION_MINOR 0)

message("source directory " ${PROJECT_SOURCE_DIR})

enable_testing()
# set(CTEST_OUTPUT_ON_FAILURE ON) # doesn't do anything

# reprocessing stuff
# configure_file (
#   "${PROJECT_SOURCE_DIR}/hello.h.in"
#   "${PROJECT_SOURCE_DIR}/hello.h"
#   )

# cmake auto-linking policy with qtmain
cmake_policy(SET CMP0020 NEW)

# qt flags
# this thing doesn't work...
# set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "autogennnn") # for shared gen files?
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# dependencies
# search path

find_package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgUtil osgGA osgViewer osgText)
find_package(Qt5 REQUIRED Core Gui Widgets Test)

include_directories("${PROJECT_BINARY_DIR}")
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

# get_target_property(QtWidgets Qt5::Core LOCATION)
# get_property(GENFOLDER GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER)
# message("autogen folder" ${AUTOGEN_TARGETS_FOLDER} "prop?" ${GENFOLDER})
# message("automoc folder" ${AUTOMOC_TARGETS_FOLDER})
# message("cmake binary" ${CMAKE_BINARY_DIR})

# for some reason find_package doesn't work for libraries
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${PROJECT_BINARY_DIR}/VSim_objects_autogen/include_Release)
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Test_DEFINITIONS})

include_directories(${PROJECT_SOURCE_DIR}/src)

# source
message(${PROJECT_SOURCE_DIR})
message("binary dir " ${PROJECT_BINARY_DIR})
file(GLOB_RECURSE VSim_src 
  src/*.h
  src/*.cpp
  )
list(FILTER VSim_src EXCLUDE REGEX ".*src/windows.*")
list(FILTER VSim_src EXCLUDE REGEX ".*src/main.cpp")
list(FILTER VSim_src EXCLUDE REGEX ".*src/deprecated/resources*")
file(GLOB_RECURSE VSim_headers
  src/*.h
  )
file(GLOB_RECURSE VSim_serializers
  src/*Serializer*.cpp
  )

message("sources - " "${VSim_src}")

# file(GLOB VSim_src 
#   "src/*.h" 
#   "src/narratives/*.h"
#   "src/*.cpp"
#   "src/narratives/*.cpp"
#   )

add_library(VSim_objects ${VSim_src})
add_executable(VSim src/main.cpp ${VSim_serializers})

# final add executable

# linking
target_link_libraries(VSim_objects Qt5::Widgets ${OPENSCENEGRAPH_LIBRARIES})
target_link_libraries(VSim VSim_objects)

# tests
set(tests
  Util_test
  Selection_test
  )
foreach(test ${tests})
  add_executable(${test} tests/${test}.cpp)
  add_test(NAME ${test} COMMAND ${test})
  add_dependencies(${test} VSim_objects)
  target_link_libraries(${test} VSim_objects)
  target_link_libraries(${test} Qt5::Widgets Qt5::Test)
  target_link_libraries(${test} ${OPENSCENEGRAPH_LIBRARIES})
endforeach(test)

#install
install(TARGETS VSim DESTINATION .)
install(DIRECTORY assets DESTINATION .)
install(FILES CHANGELOG.md COPYRIGHT LICENSE README.md THIRD-PARTY-NOTICES DESTINATION .)

if (WIN32)
  # DLL copying
  set(OPENSCENEGRAPH_BINARY_DIR "" CACHE FILEPATH "where are the osg binaries")
  if(NOT IS_DIRECTORY ${OPENSCENEGRAPH_BINARY_DIR})
    message(${OPENSCENEGRAPH_BINARY_DIR})
    message(FATAL_ERROR "Must define OPENSCENEGRAPH_BINARY_DIR")
  endif()

  set(THIRDPARTY_BINARY_DIR "" CACHE FILEPATH "where are the osg dependency binaries (zlib and stuff)")
  if(NOT IS_DIRECTORY ${THIRDPARTY_BINARY_DIR})
    message(${THIRDPARTY_BINARY_DIR})
    message(FATAL_ERROR "Must define THIRDPARTY_BINARY_DIR")
  endif()

  set(QT_DIR "" CACHE FILEPATH "where is the Qt root dir")
  if(NOT IS_DIRECTORY ${QT_DIR})
    message(${QT_DIR})
    message(FATAL_ERROR "Must define QT_DIR")
  endif()

  # visual studio path https://stackoverflow.com/questions/28533012/how-to-set-runtime-path-for-cmake-custom-command-on-windows
  # set(CMAKE_MSVCIDE_RUN_PATH "${OPENSCENEGRAPH_BINARY_DIR};${OPENSCENEGRAPH_BINARY_DIR}/osgPlugins-3.4.0/;${THIRDPARTY_BINARY_DIR};${QT_DIR}/bin/")
  # message("msvc path" ${CMAKE_MSVCIDE_RUN_PATH})

  # osg dlls
  file(GLOB DLL_SEARCH ${OPENSCENEGRAPH_BINARY_DIR}/*.dll)
  # take off the path & extension -> DLL_LIST
  list(FILTER DLL_SEARCH EXCLUDE REGEX ".*d.dll")
  foreach(DLL ${DLL_SEARCH})
    get_filename_component(COMP ${DLL} NAME_WE)
	list(APPEND DLL_LIST ${COMP})
  endforeach(DLL)
  message("DLL LIST:\n" "${DLL_LIST}")

  # find the plugin directory
  file(GLOB OSG_PLUGIN_DIR ${OPENSCENEGRAPH_BINARY_DIR}/osgPlugins*)
  get_filename_component(OSG_PLUGIN_PREFIX ${OSG_PLUGIN_DIR} NAME)
  message("PLUGIN PREFIX: " ${OSG_PLUGIN_PREFIX})
  # do the same thing as before -> PLUGIN_LIST
  file(GLOB PLUGIN_SEARCH ${OSG_PLUGIN_DIR}/*.dll)
  list(FILTER PLUGIN_SEARCH EXCLUDE REGEX ".*d.dll")
  foreach(DLL ${PLUGIN_SEARCH})
    get_filename_component(COMP ${DLL} NAME_WE)
	list(APPEND PLUGIN_LIST ${COMP})
  endforeach(DLL)
  message("PLUGIN LIST:\n" "${PLUGIN_LIST}")

  set(THIRDPARTYDLLS
   libpng16
   zlib
   )
  add_custom_command(
    TARGET VSim
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:VSim>/${OSG_PLUGIN_PREFIX}
  )

  # copy thirdparty dlls on postbuild
  foreach(DLL ${THIRDPARTYDLLS})
    set(PREFIX ${THIRDPARTY_BINARY_DIR}/${DLL})
    add_custom_command(
     TARGET VSim
     PRE_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll
         $<TARGET_FILE_DIR:VSim>
    )
    install(FILES ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll DESTINATION ".")
  endforeach(DLL)
  
  # copy OSG dlls on postbuild
  foreach(DLL ${DLL_LIST})
    set(PREFIX ${OPENSCENEGRAPH_BINARY_DIR}/${DLL})
   add_custom_command(
     TARGET VSim 
     PRE_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll
         $<TARGET_FILE_DIR:VSim>
   )
    install(FILES ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll DESTINATION ".")
  endforeach(DLL)

  # copy OSG plugin dlls
  foreach(DLL ${PLUGIN_LIST})
    set(PREFIX ${OPENSCENEGRAPH_BINARY_DIR}/${OSG_PLUGIN_PREFIX}/${DLL})
   add_custom_command(
     TARGET VSim
     POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll
         $<TARGET_FILE_DIR:VSim>/${OSG_PLUGIN_PREFIX}/
   )
    install(FILES ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll DESTINATION "${OSG_PLUGIN_PREFIX}/")
  endforeach(DLL)

  # Qt
  foreach(dll
      Qt5::Core
      Qt5::Gui
      Qt5::Widgets
	  Qt5::Test
      )
    # copy to the Release directory
    add_custom_command(
      TARGET VSim
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:${dll}>
        $<TARGET_FILE_DIR:VSim>
    )
	# if you want the debug dll location
	#get_target_property(debug_path ${dll} IMPORTED_LOCATION_DEBUG)
	#$<$<CONFIG:Debug>:${debug_path}>$<$<NOT:$<CONFIG:Debug>>:${release_path}>
    # copy to the install directory
    install(FILES $<TARGET_FILE:${dll}> DESTINATION ".")
    message("imported target: ${dll}, path: ${dll_location}")
  endforeach(dll)
  
  # Qt plugins
  foreach(dll
      ${Qt5Gui_PLUGINS}
      )
    get_target_property(loc ${dll} LOCATION)
     # get the relative path, something like plugins/imageformats/png.dll
    file(RELATIVE_PATH rel ${QT_DIR} ${loc})
    # then extract the directory - plugins/imageformats
    get_filename_component(directory ${rel} DIRECTORY)
    # finally add it to the install
    install(FILES $<TARGET_FILE:${dll}> DESTINATION ${directory})
    message("imported target: ${dll}, path: ${dll_location}")
  endforeach(dll)

  # windows platform
  install(FILES ${QT_DIR}/plugins/platforms/qwindows.dll DESTINATION platforms)
endif(WIN32)

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
if (WIN32)
  set(CPACK_GENERATOR WIX)
endif (WIN32)
if (APPLE)
  set(CPACK_BINARY_DRAGNDROP ON)
  #set(CPACK_BINARY_BUNDLE ON)
endif (APPLE)

set(CPACK_PACKAGE_NAME "VSim")
set(CPACK_PACKAGE_VENDOR "idre.ucla.edu")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VSim - 3D viewer and presentation tool")
set(CPACK_PACKAGE_VERSION "${VSim_VERSION_MAJOR}.${VSim_VERSION_MINOR}.0")
set(CPACK_PACKAGE_VERSION_MAJOR ${VSim_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VSim_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "VSim")

include(CPack)
