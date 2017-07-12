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

find_package(OpenSceneGraph 3.0.0 REQUIRED osgDB osgUtil)
find_package(Qt5Widgets)

include_directories("${PROJECT_BINARY_DIR}")
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})
#get_target_property(QtWidgets Qt5::Core LOCATION)

# source
file(GLOB hello_src "*.h" "*.cpp")

# final add executable
add_executable(hello ${hello_src})

# linking
target_link_libraries(hello Qt5::Widgets)
target_link_libraries(hello ${OPENSCENEGRAPH_LIBRARIES})

#install
install(TARGETS hello DESTINATION ".")
install(DIRECTORY assets DESTINATION .)


# DLL copying
set(OPENSCENEGRAPH_BINARY_DIR "" CACHE FILEPATH "where are the osg binaries")
if(NOT IS_DIRECTORY ${OPENSCENEGRAPH_BINARY_DIR})
  message(${OPENSCENEGRAPH_BINARY_DIR})
  message(FATAL_ERROR "Must define OPENSCENEGRAPH_BINARY_DIR")
endif()

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
  #osgdb_geo
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
  #osgdb_p3d
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
  TARGET hello 
  PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
      $<TARGET_FILE_DIR:hello>/${OPENSCENEGRAPH_PLUGIN_PREFIX}
)

# copy OSG dlls on postbuild
foreach(OSGDLL ${OSGDLLS})
  add_custom_command(
    TARGET hello 
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OPENSCENEGRAPH_BINARY_DIR}/${OSGDLL}$<$<CONFIG:DEBUG>:d>.dll
        $<TARGET_FILE_DIR:hello>
  )
endforeach(OSGDLL)

# copy OSG plugin dlls
foreach(OSGDLL ${OSGPLUGINDLLS})
  add_custom_command(
    TARGET hello 
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OPENSCENEGRAPH_BINARY_DIR}/${OPENSCENEGRAPH_PLUGIN_PREFIX}/${OSGDLL}$<$<CONFIG:DEBUG>:d>.dll
        $<TARGET_FILE_DIR:hello>/${OPENSCENEGRAPH_PLUGIN_PREFIX}/
  )
endforeach(OSGDLL)

# Qt
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
  install(FILES ${dll_location} DESTINATION ".")
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
