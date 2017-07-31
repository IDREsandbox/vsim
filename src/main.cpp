#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow window(NULL);

	window.show();
	VSimApp vsim(&window);

	vsim.openVSim("assets/default.vsim");
	//vsim.openVSim("T:/Projects/_UCLA/NewRunTimeSoftware/ModelTests/Pantheon/Pantheon_v1_wNarratives.vsim");
	return a.exec();
}
