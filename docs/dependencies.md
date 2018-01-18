# Building OpenSceneGraph

```
git clone https://github.com/openscenegraph/OpenSceneGraph.git
cd OpenSceneGraph
git checkout tags/OpenSceneGraph-3.5.6 #or whatever version you want
```

## Dependencies file structure
  3rdParty-build
    x64
  OpenSceneGraph (source)
  osg-build
    install
  
## Building third party

Don't build third party, it's too hard -.-

Download 2017 third party packages here
https://www.osgvisual.org/wiki/Downloads

## in the dependencies root directory
mkdir osg-build
cd osg-build
THIRDPARTY=C:/Users/David/Desktop/dependencies/3rdParty-build/x64
cmake -G "Visual Studio 15 2017 Win64"\
  -DACTUAL_3RDPARTY_DIR=$THIRDPARTY\
  -DCOLLADA_BOOST_FILESYSTEM_LIBRARY=$THIRDPARTY/lib/boost_filesystem-vc141-mt-1_63.lib\
  -DCOLLADA_BOOST_FILESYSTEM_LIBRARY_DEBUG=$THIRDPARTY/lib/boost_container-vc141-mt-gd-1_63.lib\
  -DCOLLADA_BOOST_SYSTEM_LIBRARY=$THIRDPARTY/lib/boost_system-vc141-mt-1_63.lib\
  -DCOLLADA_BOOST_SYSTEM_LIBRARY_DEBUG=$THIRDPARTY/lib/boost_system-vc141-mt-gd-1_63.lib\
  -DCOLLADA_DOM_ROOT=$THIRDPARTY/include\
  -DCOLLADA_DYNAMIC_LIBRARY=$THIRDPARTY/lib/collada-dom2.4-dp-vc141-mt.lib\
  -DCOLLADA_DYNAMIC_LIBRARY_DEBUG=$THIRDPARTY/lib/collada-dom2.4-dp-vc141-mt-d.lib\
  -DCOLLADA_INCLUDE_DIR=$THIRDPARTY/include\
  -DBUILD_OSG_PLUGIN_NVTT=0\
  ../OpenSceneGraph

How to exclude broken plugins:
-DBUILD_OSG_PLUGIN_NVTT=0

## build it
cmake --build .
cmake --build . --config RELEASE
cmake --build . --config DEBUG
cmake --build . --config RELEASE --target INSTALL