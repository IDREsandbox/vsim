cmake_minimum_required (VERSION 2.8.11)
project (VSim)

# version number
set (VSim_VERSION_MAJOR 2)
set (VSim_VERSION_MINOR 0)
set (VSim_VERSION_REVISION 0)

message("source directory " ${PROJECT_SOURCE_DIR})

enable_testing()
# set(CTEST_OUTPUT_ON_FAILURE ON) # doesn't do anything

# preprocessing stuff
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

#
if (WIN32)
  set(QT_DIR "" CACHE FILEPATH "where is the Qt install directory")
  if(NOT IS_DIRECTORY ${QT_DIR})
    message("QT_DIR: ${QT_DIR}")
    message(FATAL_ERROR "Must define QT_DIR")
  endif()

  set(OSG_DIR "" CACHE FILEPATH "where is the osg install directory")
  if(NOT IS_DIRECTORY ${OSG_DIR})
    message("OSG_DIR: ${OSG_DIR}")
    message(FATAL_ERROR "Must define OSG_DIR")
  endif()

  set(TP_BIN "" CACHE FILEPATH "where are the osg dependency binaries (zlib and stuff)")
  if(NOT IS_DIRECTORY ${TP_BIN})
    message("TP_BIN: ${TP_BIN}")
    message(FATAL_ERROR "Must define TP_BIN")
  endif()
endif()

# set CMAKE_PREFIX_PATH to install directories for find_package
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${OSG_DIR};${QT_DIR};)

# set osg binary dir for dll copying
message("QT_DIR:" "${QT_DIR}")
message("OSG_DIR:" "${OSG_DIR}")
message("TP_BIN:" "${TP_BIN}")
set(OSG_BIN "${OSG_DIR}/bin")
message("OSG_BIN:" "${OSG_BIN}")

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

# dll copy stuff
add_custom_target(CopyDLLs)

# source
message(${PROJECT_SOURCE_DIR})
message("binary dir " ${PROJECT_BINARY_DIR})
file(GLOB_RECURSE VSim_src
  src/*.h
  src/*.cpp
  )
list(FILTER VSim_src EXCLUDE REGEX ".*src/windows/*")
list(FILTER VSim_src EXCLUDE REGEX ".*src/main.cpp")
file(GLOB_RECURSE VSim_headers
  src/*.h
  )
file(GLOB_RECURSE VSim_serializers
  src/*Serializer*.cpp
  )

message("SOURCES:\n" "${VSim_src}")
message("SERIALIZERS:\n" "${VSim_serializers}")

# file(GLOB VSim_src
#   "src/*.h"
#   "src/narratives/*.h"
#   "src/*.cpp"
#   "src/narratives/*.cpp"
#   )

# add_custom_target(Meme)

add_library(VSim_objects ${VSim_src})
add_executable(VSim src/main.cpp ${VSim_serializers})
add_dependencies(VSim CopyDLLs)
# add_dependencies(VSim Meme);

# final add executable

# linking
target_link_libraries(VSim_objects Qt5::Widgets ${OPENSCENEGRAPH_LIBRARIES})
target_link_libraries(VSim VSim_objects)

# gui tests (experiments) ##################
# add_subdirectory("experiments")

set(NumberSlider_src
  src/NumberSlider.cpp
  experiments/NumberSlider_experiment/NumberSlider_experiment.cpp
)
message("exp sources ----:\n" "${NumberSlider_src}")
add_executable(NumberSlider_experiment ${NumberSlider_src})
target_link_libraries(NumberSlider_experiment Qt5::Widgets)

set(HorizontalScrollBox_src
  src/HorizontalScrollBox.h
  src/HorizontalScrollBox.cpp
  src/ScrollBoxItem.h
  src/ScrollBoxItem.cpp
  src/Selection.h
  src/Selection.cpp
  src/SelectionStack.h
  src/SelectionStack.cpp
  src/Util.h
  src/Util.cpp
  experiments/HorizontalScrollBox_experiment/HorizontalScrollBox_experiment.cpp
)
add_executable(HorizontalScrollBox_experiment ${HorizontalScrollBox_src})
target_link_libraries(HorizontalScrollBox_experiment Qt5::Widgets)
target_link_libraries(HorizontalScrollBox_experiment ${OPENSCENEGRAPH_LIBRARIES})

set(Canvas_src
  src/narrative/CanvasContainer.h
  src/narrative/CanvasContainer.cpp
  experiments/Canvas_experiment/Canvas_experiment.cpp
)
add_executable(Canvas_experiment ${Canvas_src})
target_link_libraries(Canvas_experiment Qt5::Widgets)
target_link_libraries(Canvas_experiment ${OPENSCENEGRAPH_LIBRARIES})

# tests ####################################

# util test & dependencies
set(Util_src
  tests/Util_test.cpp
  src/Util.cpp
  src/Util.h)
add_executable(Util_test ${Util_src})
add_test(NAME Util_test COMMAND Util_test)
target_link_libraries(Util_test Qt5::Widgets Qt5::Test)
target_link_libraries(Util_test ${OPENSCENEGRAPH_LIBRARIES})

# group test
set(Group_src
  tests/Group_test.cpp
  src/Group.cpp
  src/Group.h)
add_executable(Group_test ${Group_src})
add_test(NAME Group_test COMMAND Group_test)
target_link_libraries(Group_test Qt5::Widgets Qt5::Test)
target_link_libraries(Group_test ${OPENSCENEGRAPH_LIBRARIES})

# er filter proxy test
set(ERFilterSortProxy_src
  tests/ERFilterSortProxy_test.cpp
  src/Group.cpp
  src/Group.h
  src/resources/ERFilterSortProxy.h
  src/resources/ERFilterSortProxy.cpp
  src/resources/ECategory.h
  src/resources/ECategory.cpp
  src/resources/ECategoryGroup.h
  src/resources/ECategoryGroup.cpp
  src/resources/EResource.h
  src/resources/EResource.cpp
  src/resources/EResourceGroup.h
  src/resources/EResourceGroup.cpp
  src/CheckableListProxy.h
  src/CheckableListProxy.cpp
  src/GroupModel.h
  src/GroupModel.cpp
  src/Util.h
  src/Util.cpp
  src/Command.h
  )
add_executable(ERFilterSortProxy_test ${ERFilterSortProxy_src})
add_test(NAME ERFilterSortProxy_test COMMAND ERFilterSortProxy_test)
target_link_libraries(ERFilterSortProxy_test Qt5::Widgets Qt5::Test)
target_link_libraries(ERFilterSortProxy_test ${OPENSCENEGRAPH_LIBRARIES})

set(TEST_LIST
  Selection_test
  )
# file(GLOB TEST_SEARCH tests/*.cpp)
# foreach(TEST_SRC ${TEST_SEARCH})
#   get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
#   list(APPEND TEST_LIST ${TEST_NAME})
# endforeach(TEST_SRC)

foreach(test ${TEST_LIST})
  message("add test: " ${test})
  # EXCLUDE_FROM_ALL
  add_executable(${test} tests/${test}.cpp)
  add_test(NAME ${test} COMMAND ${test})
  target_link_libraries(${test} VSim_objects)
  target_link_libraries(${test} Qt5::Widgets Qt5::Test)
  target_link_libraries(${test} ${OPENSCENEGRAPH_LIBRARIES})
endforeach(test)

#install
install(TARGETS VSim DESTINATION .)
install(DIRECTORY assets DESTINATION .)
install(FILES CHANGELOG.md COPYRIGHT LICENSE README.md THIRD-PARTY-NOTICES DESTINATION .)

if (WIN32)
  # visual studio path https://stackoverflow.com/questions/28533012/how-to-set-runtime-path-for-cmake-custom-command-on-windows
  # set(CMAKE_MSVCIDE_RUN_PATH "${OPENSCENEGRAPH_BINARY_DIR};${OPENSCENEGRAPH_BINARY_DIR}/osgPlugins-3.4.0/;${THIRDPARTY_BINARY_DIR};${QT_DIR}/bin/")
  # message("msvc path" ${CMAKE_MSVCIDE_RUN_PATH})

  # osg dll list
  file(GLOB OSG_DLL_SEARCH ${OSG_BIN}/*.dll)
  # take off the path & extension -> DLL_LIST
  list(FILTER OSG_DLL_SEARCH EXCLUDE REGEX ".*d.dll")
  set(OSG_DLL_LIST "")
  foreach(DLL ${OSG_DLL_SEARCH})
    get_filename_component(COMP ${DLL} NAME_WE)
	list(APPEND OSG_DLL_LIST ${COMP})
  endforeach(DLL)
  message("OSG DLL LIST:\n" "${OSG_DLL_LIST}")

  # copy OSG dlls on postbuild
  foreach(DLL ${OSG_DLL_LIST})
    set(PREFIX ${OSG_BIN}/${DLL})
   add_custom_command(
     TARGET CopyDLLs
     PRE_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll
         $<CONFIG>
   )
    install(FILES ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll DESTINATION ".")
  endforeach(DLL)

  # find the osg plugin directory
  file(GLOB OSG_PLUGIN_DIR ${OSG_BIN}/osgPlugins*)
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

  # create the plugin directory
  add_custom_command(
    TARGET CopyDLLs
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<CONFIG>/${OSG_PLUGIN_PREFIX}
  )

  # copy OSG plugin dlls
  foreach(DLL ${PLUGIN_LIST})
    set(PREFIX ${OSG_BIN}/${OSG_PLUGIN_PREFIX}/${DLL})
   add_custom_command(
     TARGET CopyDLLs
     POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll
         $<CONFIG>/${OSG_PLUGIN_PREFIX}/
   )
    install(FILES ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll DESTINATION "${OSG_PLUGIN_PREFIX}/")
  endforeach(DLL)

  # third party dll list
  set(THIRDPARTYDLLS
   libpng16
   zlib
   )

  # third party dll copying
  foreach(DLL ${THIRDPARTYDLLS})
    set(PREFIX ${TP_BIN}/${DLL})
    add_custom_command(
     TARGET CopyDLLs
     PRE_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll
         $<CONFIG>
    )
    install(FILES ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll DESTINATION ".")
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
      TARGET CopyDLLs
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${dll}>
        $<CONFIG>
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
    #message("imported target: ${dll}, path: ${dll_location}")
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
set(CPACK_PACKAGE_VERSION "${VSim_VERSION_MAJOR}.${VSim_VERSION_MINOR}.${VSim_VERSION_REVISION}")
set(CPACK_PACKAGE_VERSION_MAJOR ${VSim_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VSim_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VSim_VERSION_REVISION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "VSim")

include(CPack)
