cmake_minimum_required (VERSION 3.7)
project (VSim)

# version number
set (VSim_VERSION_MAJOR 2)
set (VSim_VERSION_MINOR 0)
set (VSim_VERSION_REVISION 0)

set (CMAKE_CXX_STANDARD 17)

enable_testing()
# set(CTEST_OUTPUT_ON_FAILURE ON) # doesn't do anything

# preprocessing stuff
# configure_file (
#   "${PROJECT_SOURCE_DIR}/hello.h.in"
#   "${PROJECT_SOURCE_DIR}/hello.h"
#   )

# cmake auto-linking policy with qtmain
cmake_policy(SET CMP0020 NEW)

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

find_package(Assimp REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIRS})

if(APPLE)
  set(CMAKE_PREFIX_PATH "/usr/local/opt/qt;${CMAKE_PREFIX_PATH}")
endif()

find_package(Qt5 REQUIRED Core Gui Widgets Test Concurrent)
find_package(Qt5Concurrent REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Concurrent_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Concurrent_INCLUDE_DIRS})

if (APPLE)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
  find_package(cryptopp MODULE REQUIRED)
  set(CRYPTOPP_DEP ${CRYPTOPP_LIBRARIES})
else()
  find_package(cryptopp CONFIG REQUIRED)
  set(CRYPTOPP_DEP cryptopp-static)
endif()

# qt flags
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# these don't work
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "autouic")
#set_property(GLOBAL PROPERTY AUTOGEN_BUILD_DIR "aaaaaaaaaa")
#message("GAUTOGEN:\n" ${gautogen})
# set(CMAKE_AUTORCC ON)

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

include_directories (${CMAKE_BINARY_DIR}/generated)

# input: foo.fbs (caller relative path, uses CMAKE_CURRENT_SOURCE_DIR)
# output: var is set to output header absolute path (foo_generated.h)
function(vsim_add_flatbuffer var fbs_src)
  set(fbs_full ${CMAKE_CURRENT_SOURCE_DIR}/${fbs_src})
  get_filename_component(fbs ${fbs_full} NAME)
  string(REGEX REPLACE "\\.fbs$" "_generated.h" fbs_header ${fbs})
  set(fbs_header_full ${CMAKE_BINARY_DIR}/generated/${fbs_header})
  #list(APPEND fbs_header_list ${fbs_header_full})
  # some annoying qt stuff
  set_property(SOURCE ${fbs_header_full} PROPERTY SKIP_AUTOGEN ON)
  add_custom_command(
    OUTPUT ${fbs_header_full}
    COMMAND echo "Compiling flatbuffer - ${fbs}"
    COMMAND ${flatcExec} -c
            --gen-object-api
            -o ${CMAKE_BINARY_DIR}/generated
            -I ${CMAKE_SOURCE_DIR}/src
            ${fbs_full})
  set(${var} ${fbs_header_full} PARENT_SCOPE)
endfunction()

function(vsim_add_test name src libs)
  message("adding test: ${name}")
  add_executable(${name}
    EXCLUDE_FROM_ALL # do we exclude from all?
    ${src})
  # message("name?" "${name}" "\nsrc?" "${src}")
  add_test(NAME ${name} COMMAND ${name})
  # message("libraries?" "${libs}")
  target_link_libraries(${name} ${libs})
endfunction()

function(vsim_add_experiment name src libs)
  message("adding experiment: ${name}")
  add_executable(${name}
    EXCLUDE_FROM_ALL
    ${src})
  target_link_libraries(${name} ${libs})
  set_target_properties(${name} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endfunction()

# if you have .ui files in a library
# then use this to propagate the include dirs
# qt auto_uic doesn't do this automatically (I think?)
function(vsim_include_uic target)
  # message("generator: ${CMAKE_GENERATOR}")
  # message("compiler id: ${CMAKE_CXX_COMPILER_ID}")

  # the qt autogen path changes 
  # (use generator expressions to figure this out?)
  # The qt docs: the include dir is _autogen/include or include_<config>
  if("${GENERATOR_IS_MULTI_CONFIG}" # I think GENERATOR_IS_MULTI_CONFIG is broken
      OR (${CMAKE_GENERATOR} STREQUAL "Xcode")
      OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")) # for some reason this doesn't work w/o quotes
    set(includeSuffix "include_$<CONFIG>")
  else()
    set(includeSuffix "include")
  endif()

  set(extraIncludeDir "${CMAKE_CURRENT_BINARY_DIR}/${target}_autogen/${includeSuffix}")
  message("including uic dir ${extraIncludeDir}")
  target_include_directories(${target} PUBLIC "${extraIncludeDir}")
endfunction()

# puts all the tests and things in build/Debug
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D GL_SILENCE_DEPRECATION")
  SET(CMAKE_EXE_LINKER_FLAGS "-v -L/usr/local/lib")
  add_compile_definitions(GL_SILENCE_DEPRECATION)
  message("setting special flags" ${CMAKE_CXX_FLAGS})
endif()

include_directories(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(src)

#install
install(FILES CHANGELOG.md COPYRIGHT LICENSE README.md THIRD-PARTY-NOTICES DESTINATION .)

#assets copying
file(COPY assets DESTINATION "${CMAKE_BINARY_DIR}")

add_custom_command(
  OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/assets
  COMMAND echo "Copying assets"
  COMMAND COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/assets)
add_custom_target(CopyAssets
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/assets)

install(DIRECTORY assets DESTINATION .)

if (WIN32)

  # install dlls
  # hmm, well I guess it works...
  install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/
    DESTINATION .
    FILES_MATCHING PATTERN "*.dll")

  set(vcpkgBin ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin)  
  file(GLOB osgSearch ${vcpkgBin}/osg*.dll) # osgSearch: /stuff/bin/osg148-osg.dll
  set(osgList "") # osgList: osg148-osg
  foreach(dll ${osgSearch})
    get_filename_component(comp ${dll} NAME_WE)
    list(APPEND osgList ${comp})
  endforeach()

  # copy OSG dlls on postbuild
  add_custom_target(CopyBuildDLLs)
  foreach(dll ${osgList})
    set(PREFIX ${OSG_BIN}/${DLL})
    add_custom_command(
      TARGET CopyBuildDLLs
      COMMAND echo "Copying osg dll ${dll}"
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}$<$<CONFIG:Debug>:/debug>/bin/${dll}$<$<CONFIG:Debug>:d>.dll
        $<CONFIG>
    )
    install(FILES ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/${dll}.dll DESTINATION ".")
  endforeach(dll)

  # copy osg plugins
  set(pluginsFolder osgPlugins-${OPENSCENEGRAPH_VERSION}) # "osgPlugins-3.4.0" or something
  find_file(osgPluginsDebug NAMES ${pluginsFolder} PATHS ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/tools/osg)
  find_file(osgPlugins NAMES ${pluginsFolder} PATHS ${_VCPKG_TOOLS_DIRS})
  file(COPY ${osgPlugins} DESTINATION "${CMAKE_BINARY_DIR}/osgPlugins")
  file(COPY ${osgPluginsDebug} DESTINATION "${CMAKE_BINARY_DIR}/osgPlugins_debug")
  install(DIRECTORY ${osgPlugins} DESTINATION .)

  add_custom_command(
    OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/${pluginsFolder}
    COMMAND echo "Copying osg plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_BINARY_DIR}/osgPlugins$<$<CONFIG:Debug>:_debug>/${pluginsFolder}
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/${pluginsFolder})
  add_custom_target(CopyOSGPlugins
    DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/${pluginsFolder})

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
