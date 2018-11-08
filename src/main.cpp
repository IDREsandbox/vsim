#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"
#include "Core/VSimInfo.h"

#include <memory>
#include <QDebug>
#include <QDir>
#include <osgDB/Registry>
#include <QLibraryInfo>

USE_OSGPLUGIN(assimp)

int main(int argc, char *argv[])
{
#if __APPLE__
	qDebug() << "__apple__";

	// mac release
	// /Contents
	//   /MacOS
	//   /Frameworks
	//   /Plugins

	// search for Info.plist
	// search for ./Frameworks, ../Frameworks, etc
	// set the BinariesPath

	qDebug() << "library paths:" << QCoreApplication::libraryPaths();

	VSimInfo::initPath(argv[0]);

	qDebug() << "assets path:" << VSimInfo::assets();
	qDebug() << "is bundle:" << VSimInfo::isMacBundle();

	// for (const char *path : {"./Frameworks", "../Frameworks"}) {
	// 	auto joined = QDir(QDir(appdir).relativeFilePath(path));
	// 	qDebug() << "testing fw dir" << joined.absolutePath();
	// 	if (joined.exists()) {
	// 		QCoreApplication::setLibraryPaths({joined.absolutePath()});
	// 		qDebug() << "setting library paths:" << joined.absolutePath();
	// 		break;
	// 	}
	// }
	// qDebug() << "library info plugins?" << QLibraryInfo::location(QLibraryInfo::PluginsPath);
	// for (const char *path : {"./Plugins", "../Plugins"}) {
	// 	auto joined = QDir(QDir(appdir).relativeFilePath(path));
	// 	qDebug() << "testing plugin dir" << joined.absolutePath();
	// 	if (joined.exists()) {
	// 		QCoreApplication::setLibraryPaths({joined.absolutePath()});
	// 	}
	// }
	qDebug() << "library location" << QLibraryInfo::location(QLibraryInfo::BinariesPath);
	qDebug() << "plugins location" << QLibraryInfo::location(QLibraryInfo::PluginsPath);
	qDebug() << "library paths?" << QCoreApplication::libraryPaths();
#endif

	QApplication a(argc, argv);

	QString assets = VSimInfo::assets();
	QString startup = assets + "/default.vsim";
	if (argc >= 2) {
		startup = argv[1];
		qInfo() << "startup set to" << startup;
	}
	//qputenv("QT_FATAL_WARNINGS", "1"); // for debugging bad connections
	//qputenv("OSG_GL_ERROR_CHECKING", "ON");

	a.setWindowIcon(QIcon(assets + "/vsim.png"));

	MainWindow window;
	VSimApp vsim(&window);

	QFile File(assets + "/style.qss");
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
