#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"

#include <memory>
#include <QDebug>

int main(int argc, char *argv[])
{
	std::string startup = "assets/default.vsim";
	if (argc >= 2) {
		startup = argv[1];
		qDebug() << "startup set to" << QString::fromStdString(startup);
	}

	QApplication a(argc, argv);
	a.addLibraryPath("plugins");
	a.setWindowIcon(QIcon("assets/vsim.png"));

	MainWindow window;
	VSimApp vsim(&window);

	QFile File("assets/style.qss");
	File.open(QFile::ReadOnly);
	QString style = QLatin1String(File.readAll());
	window.setStyleSheet(style);

	window.show();

	// TODO: remove circular dependency
	window.setApp(&vsim);
	//vsim.setWindow(&window);

	window.execOpen(startup.c_str());
	return a.exec();
}
