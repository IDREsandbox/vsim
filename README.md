#VSim

##Installation
### Windows - Visual Studio 2015
Setup
1. Install QTPackage Visual Studio plugin
2. `git clone https://github.com/IDREsandbox/vsim.git`
3. Project Settings 
	Environment - `%QTDIR%\bin;%DEPS%\OpenSceneGraph-3.4.0\build\vs2015_64\bin;%PATH%`
4. Setup a link to the dependencies 
	The project assumes that dependencies are located in the root git directory, so you have to give it a link to the actual dependencies. If you'reat the sandbox, dependencies are in vsim/vsim-dependencies.

	`mklink /D dependencies T:\Projects\_UCLA\
vsim\vsim-dependencies`