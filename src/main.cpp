#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"

#include <memory>
#include <QDebug>
#include <QDir>
#include <osgDB/Registry>
#include <QLibraryInfo>

USE_OSGPLUGIN(assimp)

int main(int argc, char *argv[])
{
	qDebug() << "library location" << QLibraryInfo::location(QLibraryInfo::BinariesPath);
	qDebug() << "plugins location" << QLibraryInfo::location(QLibraryInfo::PluginsPath);
	qDebug() << "library paths?" << QCoreApplication::libraryPaths();



#if __APPLE__
	qDebug() << "__apple__";

	// mac release
	// /Contents
	//   /MacOS
	//   /Frameworks
	//   /Plugins

	// search for ./Frameworks, ../Frameworks, etc
	// set the BinariesPath

	QString appdir = argv[0];

	for (const char *path : {"./Frameworks", "../Frameworks"}) {
		auto joined = QDir(QDir(appdir).relativeFilePath(path));
		qDebug() << "testing fw dir" << joined.absolutePath();
		if (joined.exists()) {
			QCoreApplication::setLibraryPaths({joined.absolutePath()});
			qDebug() << "setting library paths:" << joined.absolutePath();
			break;
		}
	}
#endif

	qDebug() << "creating application";
	QApplication a(argc, argv);
	qDebug() << "after creating application";
	a.addLibraryPath("plugins");
	QString dir = QCoreApplication::applicationDirPath();



	QString startup = dir + "/assets/default.vsim";
	if (argc >= 2) {
		startup = argv[1];
		qInfo() << "startup set to" << startup;
	}
	//qputenv("QT_FATAL_WARNINGS", "1"); // for debugging bad connections
	//qputenv("OSG_GL_ERROR_CHECKING", "ON");

	a.setWindowIcon(QIcon(dir + "/assets/vsim.png"));

	MainWindow window;
	VSimApp vsim(&window);

	QFile File(dir + "/assets/style.qss");
	File.open(QFile::ReadOnly);
	QString style = QLatin1String(File.readAll());
	window.setStyleSheet(style);
	QDir::setCurrent(QCoreApplication::applicationDirPath());

	// FIXME: remove circular dependency
	// TODO:
	// 
	window.setApp(&vsim);

	window.show();

	window.execOpen(startup);
	return a.exec();
}
