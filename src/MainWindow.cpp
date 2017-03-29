#include <osgViewer/Viewer>
#include "MainWindow.h"

extern osgViewer::Viewer* g_viewer = nullptr;

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent)
{
	// initialize the Qt Designer stuff
	ui.setupUi(this);

	setMinimumSize(800, 600);
	ui.statusbar->showMessage("the best status bar", 0);

	// osg viewer widget
	m_osg_widget = new OSGViewerWidget(ui.root);
	g_viewer = m_osg_widget->getViewer();
	m_osg_widget->lower();
	ui.rootLayout->addWidget(m_osg_widget, 0, 0);

	// drag widget
	m_drag_area = new QWidget(ui.root);
	ui.rootLayout->addWidget(m_drag_area, 0, 0);

	dragLabel* test = new dragLabel("drag widget", m_drag_area);
	test->setObjectName(QString::fromUtf8("label"));
	test->setGeometry(250, 250, 250, 250);

}

void MainWindow::paintEvent(QPaintEvent * event)
{
	m_drag_area->setMask(m_drag_area->childrenRegion());
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
}
