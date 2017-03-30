#include <osgViewer/Viewer>
#include <QFileDialog>
#include <QDebug>
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

	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(actionNew()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(actionOpen()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(actionSave()));
	connect(ui.actionSave_As, SIGNAL(triggered()), this, SLOT(actionSaveAs()));
	connect(ui.actionImport_Model, SIGNAL(triggered()), this, SLOT(actionImportModel()));
	connect(ui.actionQuit_2, SIGNAL(triggered()), this, SLOT(close()));

	// create vsim
	m_vsimapp = std::unique_ptr<VSimApp>(new VSimApp(this));
	m_vsimapp->init();
}

void MainWindow::paintEvent(QPaintEvent * event)
{
	m_drag_area->setMask(m_drag_area->childrenRegion());
}


void MainWindow::actionNew()
{
	qDebug("new");
}
void MainWindow::actionOpen()
{
	qDebug("open action");
	QString filename = QFileDialog::getOpenFileName(this, "Open .vsim", "", "VSim files (*.vsim)");
	qDebug() << "opening - " << filename;

}

void MainWindow::actionSave()
{
	qDebug("save");
}

void MainWindow::actionSaveAs()
{
	qDebug("saveas");
}

void MainWindow::actionImportModel()
{
	qDebug("import");
	QString filename = QFileDialog::getOpenFileName(this, "Import Model", "", "Model files (*.vsim; *.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds; *.dae)");
	qDebug() << "importing - " << filename;
	m_vsimapp->importModel(filename.toStdString());
}
