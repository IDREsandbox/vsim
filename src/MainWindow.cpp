#include <osgViewer/Viewer>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>

#include "MainWindow.h"

extern osgViewer::Viewer* g_viewer = nullptr;

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent)
{
	// initialize the Qt Designer stuff
	ui.setupUi(this);

	setMinimumSize(800, 600);
	ui.statusbar->showMessage("the best status bar", 0);
	setWindowIcon(QIcon("res/vsim.ico"));

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

	connect(ui.actionNew, &QAction::triggered, this, &MainWindow::actionNew);
	connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::actionOpen);
	connect(ui.actionSave, &QAction::triggered, this, &MainWindow::actionSave);
	connect(ui.actionSave_As, &QAction::triggered, this, &MainWindow::actionSaveAs);
	connect(ui.actionImport_Model, &QAction::triggered, this, &MainWindow::actionImportModel);
	connect(ui.actionQuit, &QAction::triggered, this, &MainWindow::close);

	connect(ui.actionError, &QAction::triggered, this, [this] { this->ErrorDialog("heyo this is quite the error message"); });
	connect(ui.actionMessage, &QAction::triggered, this, [this] { this->MessageDialog("A beautiful message"); });
	connect(ui.actionProgress, &QAction::triggered, this, [this] { this->LoadingDialog("zzz"); });

	// create vsim
	m_vsimapp = std::unique_ptr<VSimApp>(new VSimApp(this));
}

void MainWindow::ErrorDialog(const std::string & msg)
{
	QErrorMessage* dialog = new QErrorMessage(this);
	dialog->showMessage(msg.c_str());
}

void MainWindow::MessageDialog(const std::string & msg)
{
	QMessageBox::warning(this, "super warning", msg.c_str());
}

void MainWindow::LoadingDialog(const std::string & msg)
{
	//auto pd = new QProgressDialog("Loading model.", "Cancel", 0, 100, this);
	//
	printf("foostart!\n");
	auto pd = new QProgressDialog("Loading model.", "Cancel", 0, 100, this);
	pd->setValue(40);
	pd->setWindowModality(Qt::WindowModal);
	pd->setMinimumDuration(0);
	pd->show();
	connect(pd, &QProgressDialog::canceled, this, [] {printf("CLOSEDD!!!\n"); });
}

void MainWindow::paintEvent(QPaintEvent * event)
{
	m_drag_area->setMask(m_drag_area->childrenRegion());
}


void MainWindow::actionNew()
{
	qDebug("new");
	m_vsimapp->reset();
}
void MainWindow::actionOpen()
{
	qDebug("open action");
	QString filename = QFileDialog::getOpenFileName(this, "Open .vsim", "", "VSim files (*.vsim; *.osg; *.osgb);;All types (*.*)");
	qDebug() << "opening - " << filename;
	m_vsimapp->openVSim(filename.toStdString());
}

void MainWindow::actionSave()
{
	qDebug("save");
}

void MainWindow::actionSaveAs()
{
	qDebug("saveas");
	QString filename = QFileDialog::getSaveFileName(this, "Save VSim", "", "osg ascii file (*.osg);;osg binary file (*.osgb)");
	if (filename == "") {
		qDebug() << "saveas cancel";
		return;
	}
	qDebug() << "saving as - " << filename;
	m_vsimapp->saveVSim(filename.toStdString());
}

void MainWindow::actionImportModel()
{
	qDebug("import");
	QString filename = QFileDialog::getOpenFileName(this, "Import Model", "", "Model files (*.vsim; *.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds; *.dae);;All types (*.*)");
	if (filename == "") {
		qDebug() << "import cancel";
		return;
	}
	qDebug() << "importing - " << filename;
	m_vsimapp->importModel(filename.toStdString());
}
