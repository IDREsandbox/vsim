#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow window(NULL);
	window.show();
	VSimApp vsim(&window);
	return a.exec();
}
