#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"
#include "ModelTableModel.h"

#include <QTreeView>

int main(int argc, char *argv[])
{
	std::string startup = "assets/default.vsim";
	if (argc >= 2) {
		startup = argv[1];
	}
	QApplication a(argc, argv);
	a.addLibraryPath("plugins");
	a.setWindowIcon(QIcon("assets/vsim.png"));
	MainWindow window;

	window.show();
	VSimApp vsim(&window);

	vsim.openVSim(startup);
	return a.exec();
}
