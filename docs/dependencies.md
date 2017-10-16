#How to Build Dependencies
How to download and build dependencies 2017

```
git clone https://github.com/openscenegraph/OpenSceneGraph.git
cd OpenSceneGraph
git checkout tags/OpenSceneGraph-3.5.6 #or whatever version you want
```

Download 2017 third party packages here
https://www.osgvisual.org/wiki/Downloads

##Dependencies file structure
  3rdParty-build
    x64
  OpenSceneGraph (source)
  osg-build

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
  ../OpenSceneGraph
  
## build it
cmake --build .
cmake --build . --config RELEASE
cmake --build . --config DEBUG

## some notes
OSG_THIRDPARTY_DIR=... doesn't work
