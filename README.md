#VSim

##Installation
### Windows - Visual Studio 2015

Setup Qt

1. Download Qt Open Source https://www.qt.io/download-open-source/
2. Install Qt 5.8, 32 and/or 64 bit versions
3. In Visual Studio - 
	- Install QTPackage plugin (Tools > Extensions and Updates)
	- QT5 > QtOptions - add the Qt versions


Setup Visual Studio

1. `git clone https://github.com/IDREsandbox/vsim.git`, the solution is in src/windows
2. PATH - vsim project > properties > Debugging > Environment
	`$(QTDIR)\bin;$(DEPS)\OpenSceneGraph-3.4.0\build\vs2015_64\bin;%PATH%`
3. Dependencies - setup a symlink to the dependencies 
	The project assumes that dependencies are located in the root git directory, so you have to give it a link to the actual dependencies. If you're at the sandbox, dependencies are in vsim/vsim-dependencies.
	`mklink /D dependencies T:\Projects\_UCLA\
vsim\vsim-dependencies`

4. Qt Version - vsim project > Qt Project Settings >	Version > link to the corresponding version (have to do this for 32 and 64 if you have both)

