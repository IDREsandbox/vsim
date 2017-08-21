#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"
#include "ModelTableModel.h"

#include <QTreeView>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow window(NULL);
	a.addLibraryPath("plugins");

	window.show();
	VSimApp vsim(&window);

	vsim.openVSim("assets/default.vsim");
	return a.exec();
}
