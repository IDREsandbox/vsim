#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"

#include <memory>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.addLibraryPath("plugins");
	QString dir = QCoreApplication::applicationDirPath();

	QString startup = dir + "/assets/default.vsim";
	if (argc >= 2) {
		startup = argv[1];
		qInfo() << "startup set to" << startup;
	}
	//qputenv("QT_FATAL_WARNINGS", "1"); // for debugging bad connections
	
	a.setWindowIcon(QIcon(dir + "/assets/vsim.png"));

	MainWindow window;
	VSimApp vsim(&window);

	QFile File(dir + "/assets/style.qss");
	File.open(QFile::ReadOnly);
	QString style = QLatin1String(File.readAll());
	window.setStyleSheet(style);

	// FIXME: remove circular dependency
	// TODO:
	// 
	window.setApp(&vsim);

	window.show();

	window.execOpen(startup);
	return a.exec();
}
