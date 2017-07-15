#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow window(NULL);
	window.show();
	VSimApp vsim(&window);

	vsim.importModel("T:\\Projects\\_UCLA\\vsim\\vsim-dependencies\\OpenSceneGraph-Data-3.4.0\\cow.osg");
	return a.exec();
}
