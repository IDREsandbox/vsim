#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"
#include "RegisterMetaTypes.h"

int main(int argc, char *argv[])
{
	registerMetaTypes();
	QApplication a(argc, argv);
	MainWindow window(NULL);
	a.addLibraryPath("plugins");

	window.show();
	VSimApp vsim(&window);

	vsim.openVSim("assets/default.vsim");
	//vsim.openVSim("T:/Projects/_UCLA/NewRunTimeSoftware/ModelTests/Pantheon/Pantheon_v1_wNarratives.vsim");
	return a.exec();
}
