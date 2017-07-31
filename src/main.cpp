#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "VSimApp.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow window(NULL);
	qDebug() << "ogmain" << window.m_osg_widget->format();
	QSurfaceFormat fmt = window.m_osg_widget->format();
	fmt.setSamples(4);
	qDebug() << "wwwww" << fmt.samples();

	window.show();
	qDebug() << "after show" << window.m_osg_widget->format().samples();
	VSimApp vsim(&window);

	qDebug() << "inthemainnn" << window.m_osg_widget->format();
	fmt = window.m_osg_widget->format();
	fmt.setSamples(4);
	qDebug() << "what" << fmt.samples();
	window.m_osg_widget->setFormat(fmt);

	vsim.importModel("T:\\Projects\\_UCLA\\vsim\\vsim-dependencies\\OpenSceneGraph-Data-3.4.0\\cow.osg");
	//vsim.openVSim("T:/Projects/_UCLA/NewRunTimeSoftware/ModelTests/Pantheon/Pantheon_v1_wNarratives.vsim");
	return a.exec();
}
