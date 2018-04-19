cmake_minimum_required (VERSION 3.7)
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
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTORCC ON)

set(osgComponents 
  osg
  osgAnimation
  osgDB
  osgFX
  osgGA
  osgManipulator
  osgParticle
  osgPresentation
  osgShadow
  osgSim
  osgTerrain
  osgText
  #osgUI
  osgUtil
  osgViewer
  osgVolume
  osgWidget
  OpenThreads
)
find_package(OpenSceneGraph 3.4.0 REQUIRED ${osgComponents})
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})


find_package(Qt5 REQUIRED Core Gui Widgets Test)
# find_package(Qt5Test REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})

# # if you want to print everything
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()


# flatbuffers stuff
# sources in src/*
# generated in build/generated/*
find_package(flatbuffers REQUIRED)
find_path(flatbuffers_include "flatbuffers")
include_directories(${flatbuffers_include})
find_program(flatcExec "flatc")
if (NOT flatcExec)
    message(FATAL_ERROR "flatbuffer compiler flatc not found")
endif()
message("flatc found: ${flatcExec}")

file(GLOB_RECURSE fbs_src
  src/*.fbs
  )
message("FBS SRC:\n" "${fbs_src}")
set(fbs_header_list "")
foreach(fbs_full ${fbs_src})
  # set(fbs_full ${CMAKE_CURRENT_SOURCE_DIR}/src/${fbs})
  get_filename_component(fbs ${fbs_full} NAME)
  string(REGEX REPLACE "\\.fbs$" "_generated.h" fbs_header ${fbs})
  set(fbs_header_full ${CMAKE_BINARY_DIR}/generated/${fbs_header})
  list(APPEND fbs_header_list ${fbs_header_full})
  # some annoying qt stuff
  set_property(SOURCE ${fbs_header_full} PROPERTY SKIP_AUTOGEN ON)
  add_custom_command(
    OUTPUT ${fbs_header_full}
    COMMAND echo "Compiling flatbuffer - ${fbs}"
    COMMAND ${flatcExec} -c --no-includes
            --gen-object-api
            -o ${CMAKE_BINARY_DIR}/generated
            -I ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${fbs_full}
    DEPENDS flatc ${fbs_full})
endforeach()
message("FBS HEADERS:\n" "${fbs_header_list}")
include_directories (${CMAKE_BINARY_DIR}/generated)
#add_custom_target(FBSHeaders ALL DEPENDS ${fbs_header_list})

include_directories(${PROJECT_SOURCE_DIR}/src)

# source
set(Util_src
  src/Util.cpp
  src/Util.h
  src/VecUtil.cpp
  src/VecUtil.h
  # src/Group.cpp
  # src/Group.h
  src/GroupTemplate.inl
  src/GroupTemplate.h
  src/GroupCommands.h
  src/GroupEnums.h
  src/GroupModelTemplate.inl
  src/GroupModelTemplate.h
  src/TypesSerializer.cpp
  src/TypesSerializer.h
  ${CMAKE_BINARY_DIR}/generated/types_generated.h
  src/Command.h
)
add_library(Util_obj ${Util_src})

file(GLOB_RECURSE Deprecated_src
  src/deprecated/*.h
  src/deprecated/*.cpp
  )
add_library(Deprecated_obj ${Deprecated_src})

set(ERModel_src
  src/ERSerializer.cpp
  src/ERSerializer.h
  ${CMAKE_BINARY_DIR}/generated/eresources_generated.h
  src/resources/ERFilterSortProxy.h
  src/resources/ERFilterSortProxy.cpp
  src/resources/ECategory.h
  src/resources/ECategory.cpp
  src/resources/ECategoryGroup.h
  src/resources/ECategoryGroup.cpp
  src/resources/ECategoryModel.h
  src/resources/ECategoryModel.cpp
  src/resources/EResource.h
  src/resources/EResource.cpp
  src/resources/EResourceGroup.h
  src/resources/EResourceGroup.cpp
  src/CheckableListProxy.h
  src/CheckableListProxy.cpp
)
add_library(ERModel_obj ${ERModel_src})
target_link_libraries(ERModel_obj Util_obj Deprecated_obj)

set(NarrativeModel_src
  src/NarrativeSerializer.cpp
  src/NarrativeSerializer.h
  ${CMAKE_BINARY_DIR}/generated/narratives_generated.h
  src/narrative/NarrativeGroup.cpp
  src/narrative/NarrativeGroup.h
  src/narrative/Narrative.cpp
  src/narrative/Narrative.h
  src/narrative/NarrativeSlide.cpp
  src/narrative/NarrativeSlide.h
  src/narrative/NarrativeSlideItem.cpp
  src/narrative/NarrativeSlideItem.h
  src/narrative/NarrativeSlideLabel.cpp
  src/narrative/NarrativeSlideLabel.h
  src/LabelStyle.cpp
  src/LabelStyle.h
  src/LabelStyleGroup.cpp
  src/LabelStyleGroup.h
  src/LabelType.h
)
add_library(NarrativeModel_obj ${NarrativeModel_src})
target_link_libraries(NarrativeModel_obj Util_obj Deprecated_obj)

set(VSimRoot_src
  src/VSimRoot.cpp
  src/VSimRoot.h
  src/VSimSerializer.cpp
  src/VSimSerializer.h
  src/Model.cpp
  src/Model.h
  src/ModelGroup.cpp
  src/ModelGroup.h
  src/OSGGroupModel.cpp
  src/OSGGroupModel.h
  src/OSGYearModel.cpp
  src/OSGYearModel.h
  src/FileUtil.cpp
  src/FileUtil.h
  ${CMAKE_BINARY_DIR}/generated/vsim_generated.h
  ${CMAKE_BINARY_DIR}/generated/settings_generated.h
)
add_library(VSimRoot_obj ${VSimRoot_src})
target_link_libraries(VSimRoot_obj Util_obj Deprecated_obj NarrativeModel_obj ERModel_obj)

set(VSim_src
  src/resources/ECategoryControl.cpp
  src/resources/ECategoryControl.h
  src/resources/ERControl.cpp
  src/resources/ERControl.h
  src/resources/ERDialog.cpp
  src/resources/ERDialog.h
  src/resources/ERDisplay.cpp
  src/resources/ERDisplay.h
  src/resources/ERFilterArea.ui
  src/resources/ERFilterArea.cpp
  src/resources/ERFilterArea.h
  src/resources/ERScrollBox.cpp
  src/resources/ERScrollBox.h
  src/resources/ERScrollItem.cpp
  src/resources/ERScrollItem.h
  src/resources/NewCatDialog.cpp
  src/resources/NewCatDialog.h

  src/narrative/CanvasContainer.h
  src/narrative/CanvasContainer.cpp
  src/narrative/NarrativeCanvas.h
  src/narrative/NarrativeCanvas.cpp
  src/narrative/NarrativeControl.h
  src/narrative/NarrativeControl.cpp
  src/narrative/NarrativeInfoDialog.h
  src/narrative/NarrativeInfoDialog.cpp
  src/narrative/NarrativePlayer.cpp
  src/narrative/NarrativePlayer.h
  src/narrative/NarrativeScrollBox.cpp
  src/narrative/NarrativeScrollBox.h
  src/narrative/NarrativeScrollItem.cpp
  src/narrative/NarrativeScrollItem.h
  src/narrative/NarrativeSlideDurationDialog.cpp
  src/narrative/NarrativeSlideDurationDialog.h
  src/narrative/SlideScrollBox.cpp
  src/narrative/SlideScrollBox.h
  src/narrative/SlideScrollItem.cpp
  src/narrative/SlideScrollItem.h

  src/BaseFirstPersonManipulator.cpp
  src/BaseFirstPersonManipulator.h
  src/editButtons.cpp
  src/editButtons.h
  src/EditDeleteDelegate.cpp
  src/EditDeleteDelegate.h
  src/ERBar.cpp
  src/ERBar.h
  src/FastScrollBox.cpp
  src/FastScrollBox.h
  src/FileUtil.cpp
  src/FileUtil.h
  src/FirstPersonManipulator.cpp
  src/FirstPersonManipulator.h
  src/FlightManipulator.cpp
  src/FlightManipulator.h
  src/FocusFilter.cpp
  src/FocusFilter.h
  src/GroupScrollBox.inl
  src/GroupScrollBox.h
  src/HorizontalScrollBox.cpp
  src/HorizontalScrollBox.h
  src/KeyTracker.cpp
  src/KeyTracker.h
  src/MainWindow.cpp
  src/MainWindow.h
  src/MainWindowTopBar.cpp
  src/MainWindowTopBar.h
  src/ModelInformationDialog.cpp
  src/ModelInformationDialog.h
  src/ModelOutliner.cpp
  src/ModelOutliner.h
  src/mrichtextedit.cpp
  src/mrichtextedit.h
  src/mtextedit.cpp
  src/mtextedit.h
  src/Navigation.h
  src/NavigationControl.cpp
  src/NavigationControl.h
  src/NumberSlider.cpp
  src/NumberSlider.h
  src/ObjectManipulator.cpp
  src/ObjectManipulator.h
  src/OSGViewerWidget.cpp
  src/OSGViewerWidget.h
  src/ScrollBoxItem.cpp
  src/ScrollBoxItem.h
  src/Selection.cpp
  src/Selection.h
  src/SelectionStack.cpp
  src/SelectionStack.h
  src/SimpleCameraManipulator.cpp
  src/SimpleCameraManipulator.h
  src/StackObject.inl
  src/StackObject.h
  src/StyleSettingsDialog.cpp
  src/StyleSettingsDialog.h
  src/StyleSettingsWidget.cpp
  src/StyleSettingsWidget.h
  src/TickSlider.cpp
  src/TickSlider.h
  src/TimeSlider.cpp
  src/TimeSlider.h
  src/VSimApp.cpp
  src/VSimApp.h
)
add_library(VSim_obj ${VSim_src})
message("SOURCES:\n" "${VSim_src}")
target_link_libraries(VSim_obj VSimRoot_obj)

# message(${PROJECT_SOURCE_DIR})
# message("binary dir " ${PROJECT_BINARY_DIR})
# file(GLOB_RECURSE VSim_src
#   src/*.h
#   src/*.cpp
#   )
# list(FILTER VSim_src EXCLUDE REGEX ".*src/windows/*")
# list(FILTER VSim_src EXCLUDE REGEX ".*src/main.cpp")
# file(GLOB_RECURSE VSim_headers
#   src/*.h
#   )
file(GLOB_RECURSE oldSerializers
  src/*/Serializer*.cpp
  )

message("SERIALIZERS:\n" "${oldSerializers}")

add_executable(VSim src/main.cpp ${oldSerializers})

# linking
target_link_libraries(VSim VSim_obj Qt5::Widgets ${OPENSCENEGRAPH_LIBRARIES})

# gui tests (experiments) ##################
# add_subdirectory("experiments")

set(NumberSlider_src
  src/NumberSlider.cpp
  experiments/NumberSlider_experiment/NumberSlider_experiment.cpp
)
add_executable(NumberSlider_experiment ${NumberSlider_src})
target_link_libraries(NumberSlider_experiment Qt5::Widgets)

set(TextEdit_src
  src/mtextedit.h
  src/mtextedit.cpp
  src/mrichtextedit.h
  src/mrichtextedit.cpp
  experiments/TextEdit_experiment.cpp
)
add_executable(TextEdit_experiment ${TextEdit_src})
target_link_libraries(TextEdit_experiment Qt5::Widgets)

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
)

add_executable(HorizontalScrollBox_experiment ${HorizontalScrollBox_src}
  experiments/HorizontalScrollBox_experiment/HorizontalScrollBox_experiment.cpp)
target_link_libraries(HorizontalScrollBox_experiment Qt5::Widgets ${OPENSCENEGRAPH_LIBRARIES})

add_executable(HSBSpeed_experiment ${HorizontalScrollBox_src}
  experiments/HSBSpeed/HSBSpeed.cpp)
target_link_libraries(HSBSpeed_experiment Qt5::Widgets ${OPENSCENEGRAPH_LIBRARIES})

set(Canvas_src
  src/narrative/CanvasContainer.h
  src/narrative/CanvasContainer.cpp
  src/narrative/CanvasContainer.h
  src/narrative/CanvasContainer.cpp
  experiments/Canvas_experiment/Canvas_experiment.cpp
)
add_executable(Canvas_experiment ${Canvas_src})
target_link_libraries(Canvas_experiment Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})

# tests ####################################

# util test & dependencies
set(Util_src
  tests/Util_test.cpp
  src/Util.cpp
  src/Util.h
  src/VecUtil.cpp
  src/VecUtil.h)
add_executable(Util_test ${Util_src})
add_test(NAME Util_test COMMAND Util_test)
target_link_libraries(Util_test Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})

# group test
set(Group_src
  tests/Group_test.cpp
  )
add_executable(Group_test ${Group_src})
add_test(NAME Group_test COMMAND Group_test)
target_link_libraries(Group_test Util_obj)
target_link_libraries(Group_test Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})

# er filter proxy test
set(ERFilterSortProxy_src
  tests/ERFilterSortProxy_test.cpp
  )
add_executable(ERFilterSortProxy_test ${ERFilterSortProxy_src})
add_test(NAME ERFilterSortProxy_test COMMAND ERFilterSortProxy_test)
target_link_libraries(ERFilterSortProxy_test ERModel_obj)
target_link_libraries(ERFilterSortProxy_test Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})

set(ERSerializer_src
  tests/ERSerializer_test.cpp
)
add_executable(ERSerializer_test ${ERSerializer_src})
add_test(NAME ERSerializer_test COMMAND ERSerializer_test)
target_link_libraries(ERSerializer_test ERModel_obj Util_obj Deprecated_obj)
target_link_libraries(ERSerializer_test Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})

set(EResource_test
  tests/EResource_test.cpp
)
add_executable(EResource_test ${ERSerializer_src})
add_test(NAME EResource_test COMMAND EResource_test)
target_link_libraries(EResource_test ERModel_obj)
target_link_libraries(EResource_test Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})

set(NarrativeSerializer_src
  tests/NarrativeSerializer_test.cpp
)
add_executable(NarrativeSerializer_test ${NarrativeSerializer_src})
add_test(NAME NarrativeSerializer_test COMMAND NarrativeSerializer_test)
target_link_libraries(NarrativeSerializer_test NarrativeModel_obj)
target_link_libraries(NarrativeSerializer_test Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})

set(ModelGroup_test_src
  tests/ModelGroup_test.cpp
)
add_executable(ModelGroup_test ${ModelGroup_test_src})
add_test(NAME ModelGroup_test COMMAND ModelGroup_test)
target_link_libraries(ModelGroup_test VSimRoot_obj)
target_link_libraries(ModelGroup_test Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})

set(VSimSerializer_src
  tests/VSimSerializer_test.cpp
  ${oldSerializers}
)
add_executable(VSimSerializer_test ${VSimSerializer_src} ${oldSerializers})
add_test(NAME VSimSerializer_test COMMAND VSimSerializer_test)
target_link_libraries(VSimSerializer_test VSimRoot_obj)
target_link_libraries(VSimSerializer_test Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})

# function(add_vsim_test testName testSrc)
#   add_executable(${testName} ${testSrc})
#   add_test(NAME ${testName} COMMAND ${testName})
#   target_link_libraries(${testName} Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})
# endfunction()
# message("serializer src\n ${VSimSerializer_src}")
# add_vsim_test(VSimSerializer_test ${VSimSerializer_src})
# target_link_libraries(VSimSerializer_test VSimRoot_obj)



# tests that involve everything
set(testList
  Selection_test
  )
# file(GLOB TEST_SEARCH tests/*.cpp)
# foreach(TEST_SRC ${TEST_SEARCH})
#   get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
#   list(APPEND TEST_LIST ${TEST_NAME})
# endforeach(TEST_SRC)

foreach(test ${testList})
  message("add test: " ${test})
  # EXCLUDE_FROM_ALL
  add_executable(${test} tests/${test}.cpp)
  add_test(NAME ${test} COMMAND ${test})
  target_link_libraries(${test} VSim_obj)
  target_link_libraries(${test} Qt5::Widgets Qt5::Test)
  target_link_libraries(${test} ${OPENSCENEGRAPH_LIBRARIES})
endforeach(test)

#install
install(TARGETS VSim DESTINATION .)
install(FILES CHANGELOG.md COPYRIGHT LICENSE README.md THIRD-PARTY-NOTICES DESTINATION .)

#assets copying
file(COPY assets DESTINATION "${CMAKE_BINARY_DIR}")
install(DIRECTORY assets DESTINATION .)

if (WIN32)

  # installs dll's using vcpkg applocal tool
  add_custom_target(InstallDLLs
      COMMAND powershell -noprofile -executionpolicy Bypass -file ${_VCPKG_TOOLCHAIN_DIR}/msbuild/applocal.ps1
      -targetBinary ${CMAKE_INSTALL_PREFIX}/VSim.exe
      -installedDir "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}$<$<CONFIG:Debug>:/debug>/bin"
      -OutVariable out
  )
  set_target_properties(InstallDLLs PROPERTIES EXCLUDE_FROM_ALL TRUE)
  # runs this target after installation
  install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" --build . --target InstallDLLs --config RELEASE)")

  set(vcpkgBin ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin)  
  file(GLOB osgSearch ${vcpkgBin}/osg*.dll) # osgSearch: /stuff/bin/osg148-osg.dll
  set(osgList "") # osgList: osg148-osg
  foreach(dll ${osgSearch})
    get_filename_component(comp ${dll} NAME_WE)
    list(APPEND osgList ${comp})
  endforeach()
  message("OSG DLL LIST:\n" "${osgList}")

  # copy OSG dlls on postbuild
  add_custom_target(CopyBuildDLLs)
  foreach(dll ${osgList})
    set(PREFIX ${OSG_BIN}/${DLL})
    add_custom_command(
      TARGET CopyBuildDLLs
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}$<$<CONFIG:Debug>:/debug>/bin/${dll}$<$<CONFIG:Debug>:d>.dll
        $<CONFIG>
    )
    install(FILES ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/${dll}.dll DESTINATION ".")
  endforeach(dll)

  # copy osg plugins
  find_file(osgPluginsDebug NAMES osgPlugins-${OPENSCENEGRAPH_VERSION} PATHS ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/tools/osg)
  find_file(osgPlugins NAMES osgPlugins-${OPENSCENEGRAPH_VERSION} PATHS ${_VCPKG_TOOLS_DIRS})
  file(COPY ${osgPluginsDebug} DESTINATION "${CMAKE_BINARY_DIR}")
  file(COPY ${osgPlugins} DESTINATION "${CMAKE_BINARY_DIR}")
  install(DIRECTORY ${osgPlugins} DESTINATION .)

endif()

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
