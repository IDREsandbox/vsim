#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"
#include "Core/VSimInfo.h"

#include <memory>
#include <QDebug>
#include <QDir>
#include <osgDB/Registry>
#include <osgDB/PluginQuery>
#include <QLibraryInfo>

USE_OSGPLUGIN(assimp)

int main(int argc, char *argv[])
{
	VSimInfo::initPath(argv[0]);

#if __APPLE__
	qInfo() << "library paths:" << QCoreApplication::libraryPaths();
	qInfo() << "assets path:" << VSimInfo::assets();
	qInfo() << "is mac bundle:" << VSimInfo::isMacBundle();
	qInfo() << "qt library location:" << QLibraryInfo::location(QLibraryInfo::BinariesPath);
	qInfo() << "qt plugins location:" << QLibraryInfo::location(QLibraryInfo::PluginsPath);
	qInfo() << "qtlibrary paths:" << QCoreApplication::libraryPaths();

	auto &osg_path_list = osgDB::Registry::instance()->getDataFilePathList();
	osg_path_list.push_back(VSimInfo::osgPluginsPath().toStdString());
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

#if _WIN32
	QDir::setCurrent(QCoreApplication::applicationDirPath());
#endif

	// FIXME: remove circular dependency
	// TODO:
	// 
	window.setApp(&vsim);

	window.show();
	window.reloadStyle();
	window.execOpen(startup);
	return a.exec();
}
