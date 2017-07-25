cmake_minimum_required (VERSION 2.6)
project (VSim)

# version number
set (VSim_VERSION_MAJOR 2)
set (VSim_VERSION_MINOR 0)

message("source directory " ${PROJECT_SOURCE_DIR})

# reprocessing stuff
# configure_file (
#   "${PROJECT_SOURCE_DIR}/hello.h.in"
#   "${PROJECT_SOURCE_DIR}/hello.h"
#   )

# cmake auto-linking policy with qtmain
cmake_policy(SET CMP0020 NEW)

# qt flags
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# dependencies
# search path

find_package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgUtil osgGA osgViewer osgText)
find_package(Qt5Widgets)

include_directories("${PROJECT_BINARY_DIR}")
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

#get_target_property(QtWidgets Qt5::Core LOCATION)

#include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/src)

# source
message(${PROJECT_SOURCE_DIR})
message("binary dir " ${PROJECT_BINARY_DIR})
file(GLOB_RECURSE VSim_src 
  src/*.h
  src/*.cpp
  )
list(FILTER VSim_src EXCLUDE REGEX ".*src/windows.*") 
message("sources - " ${VSim_src})

# file(GLOB VSim_src 
#   "src/*.h" 
#   "src/narratives/*.h"
#   "src/*.cpp"
#   "src/narratives/*.cpp"
#   )

# final add executable
add_executable(VSim ${VSim_src})

# linking
target_link_libraries(VSim Qt5::Widgets)
target_link_libraries(VSim ${OPENSCENEGRAPH_LIBRARIES})

#install
install(TARGETS VSim DESTINATION .)
install(DIRECTORY assets DESTINATION .)


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

set(QT_PLATFORMS_DIR "" CACHE FILEPATH "where is qwindows.dll?")
if(NOT IS_DIRECTORY ${QT_PLATFORMS_DIR})
  message(${QT_PLATFORMS_DIR})
  message(FATAL_ERROR "Must define QT_PLATFORMS_DIR")
endif()
install(FILES ${QT_PLATFORMS_DIR}/qwindows.dll DESTINATION platforms)

set(THIRDPARTYDLLS 
  libpng
  zlib
  )
set(OSGDLLS 
  osg130-osg
  osg130-osgAnimation
  osg130-osgDB
  osg130-osgFX
  osg130-osgGA
  osg130-osgManipulator
  osg130-osgParticle
  osg130-osgPresentation
  osg130-osgShadow
  osg130-osgSim
  osg130-osgTerrain
  osg130-osgText
  osg130-osgUtil
  osg130-osgViewer
  osg130-osgVolume
  osg130-osgWidget
#  osg130-osgQt
  ot20-OpenThreads
  )
set(OSGPLUGINDLLS
  osgdb_3dc
  osgdb_3ds
  osgdb_ac
  osgdb_bmp
  osgdb_bsp
  osgdb_bvh
  osgdb_cfg
  osgdb_dds
  osgdb_deprecated_osg
  osgdb_deprecated_osganimation
  osgdb_deprecated_osgfx
  osgdb_deprecated_osgparticle
  osgdb_deprecated_osgshadow
  osgdb_deprecated_osgsim
  osgdb_deprecated_osgterrain
  osgdb_deprecated_osgtext
  osgdb_deprecated_osgviewer
  osgdb_deprecated_osgvolume
  osgdb_deprecated_osgwidget
  osgdb_dw
  osgdb_dxf
  osgdb_freetype
#  osgdb_geo
  osgdb_gif
  osgdb_glsl
  osgdb_gz
  osgdb_hdr
  osgdb_ive
  osgdb_jpeg
  osgdb_logo
  osgdb_lwo
  osgdb_lws
  osgdb_md2
  osgdb_mdl
  osgdb_normals
  osgdb_obj
  osgdb_openflight
  osgdb_osg
  osgdb_osga
  osgdb_osgshadow
  osgdb_osgterrain
  osgdb_osgtgz
  osgdb_osgviewer
#  osgdb_p3d
  osgdb_pic
  osgdb_ply
  osgdb_png
  osgdb_pnm
  osgdb_pov
  osgdb_pvr
  osgdb_revisions
  osgdb_rgb
  osgdb_rot
  osgdb_scale
  osgdb_serializers_osg
  osgdb_serializers_osganimation
  osgdb_serializers_osgfx
  osgdb_serializers_osgmanipulator
  osgdb_serializers_osgparticle
  osgdb_serializers_osgshadow
  osgdb_serializers_osgsim
  osgdb_serializers_osgterrain
  osgdb_serializers_osgtext
  osgdb_serializers_osgvolume
  osgdb_shp
  osgdb_stl
  osgdb_tga
  osgdb_tgz
  osgdb_tiff
  osgdb_trans
  osgdb_txf
  osgdb_vtf
  osgdb_x
  osgdb_zip
  )
set(OPENSCENEGRAPH_PLUGIN_PREFIX osgPlugins-3.4.0)

add_custom_command(
  TARGET VSim 
  PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
      $<TARGET_FILE_DIR:VSim>/${OPENSCENEGRAPH_PLUGIN_PREFIX}
)

# copy THIRDPARTY dlls on postbuild
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
foreach(DLL ${OSGDLLS})
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
foreach(DLL ${OSGPLUGINDLLS})
  set(PREFIX ${OPENSCENEGRAPH_BINARY_DIR}/${OPENSCENEGRAPH_PLUGIN_PREFIX}/${DLL})
  add_custom_command(
    TARGET VSim 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll
        $<TARGET_FILE_DIR:VSim>/${OPENSCENEGRAPH_PLUGIN_PREFIX}/
  )
  install(FILES ${PREFIX}$<$<CONFIG:DEBUG>:d>.dll DESTINATION "${OPENSCENEGRAPH_PLUGIN_PREFIX}/")
endforeach(DLL)

# Qt
foreach(dll 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    )
  # copy to the Release directory
  add_custom_command(
    TARGET VSim 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:${dll}>
      $<TARGET_FILE_DIR:VSim>
  )
  # copy to the install directory
  get_target_property(dll_location ${dll} LOCATION)
  install(FILES $<TARGET_FILE:${dll}> DESTINATION ".")
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
set(CPACK_PACKAGE_NAME "VSim")
set(CPACK_PACKAGE_VENDOR "idre.ucla.edu")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VSim - 3D viewer and presentation tool")
set(CPACK_PACKAGE_VERSION "${VSim_VERSION_MAJOR}.${VSim_VERSION_MINOR}.0")
set(CPACK_PACKAGE_VERSION_MAJOR ${VSim_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VSim_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "VSim")
# set(CPACK_NSIS_MODIFY_PATH ON)

include(CPack)
