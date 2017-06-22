#include <osgViewer/Viewer>
#include <iostream>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDir>

#include "MainWindow.h"

extern osgViewer::Viewer* g_viewer = nullptr;

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent)
{
	// initialize the Qt Designer stuff
	ui.setupUi(this);

	offset = 0;

	setMinimumSize(800, 600);
	ui.statusbar->showMessage("the best status bar", 0);
	setWindowIcon(QIcon("res/vsim.ico"));
	setWindowTitle("VSim");
	setAcceptDrops(true);
	qDebug() << "root: " << QDir::currentPath();

	// osg viewer widget
	m_osg_widget = new OSGViewerWidget(ui.root);
	g_viewer = m_osg_widget->getViewer();
	m_osg_widget->lower();
	ui.rootLayout->addWidget(m_osg_widget, 0, 0);

	// drag widget
	m_drag_area = new QWidget(ui.root);
	ui.rootLayout->addWidget(m_drag_area, 0, 0);

	test = new dragLabel("I listened to the thing back when with theresa but i've forgotten it all now, except for that it is goddamn excellent.", "background-color:#ffffff;color:#000000;", m_drag_area);
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

	// connect narrative pushbuttons
	connect(ui.forward, &QPushButton::clicked, this, &MainWindow::narListForward);
	connect(ui.minus, &QPushButton::clicked, this, &MainWindow::narListDelete);
	connect(ui.plus, &QPushButton::clicked, this, &MainWindow::narListAdd);
	connect(ui.pause, &QPushButton::clicked, this, &MainWindow::narListPause);
	connect(ui.open, &QPushButton::clicked, this, &MainWindow::narListOpen);
	connect(ui.info, &QPushButton::clicked, this, &MainWindow::narListInfo);

	// create vsim
	//m_vsimapp = std::unique_ptr<VSimApp>(new VSimApp(this));
}

void MainWindow::resizeEvent(QResizeEvent *event) {
	if (offset < 2) {
		test->updateParSize();
		offset++;
	}

	if (offset >= 2)
		test->mainResize();
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

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasText()) {
		qDebug() << "drag enter " << event->mimeData()->text();
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent * event)
{
	if (event->mimeData()->hasText()) {
		QString text = event->mimeData()->text();
		qDebug() << "drop file: " << text;

		if (text.startsWith("file:///")) {
			text.remove(0, 8); // remove the prefix
		}
		//m_vsimapp->openVSim(text.toStdString());
		emit sOpenFile(text.toStdString());
	}
}


void MainWindow::actionNew()
{
	qDebug("new");
	//m_vsimapp->reset();
	emit sNew();
}
void MainWindow::actionOpen()
{
	qDebug("open action");
	QString filename = QFileDialog::getOpenFileName(this, "Open .vsim", "", "VSim files (*.vsim; *.osg; *.osgt; *.osgb; );;All types (*.*)");
	if (filename == "") {
		qDebug() << "open cancel";
		return;
	}
	qDebug() << "opening - " << filename;
	
	//m_vsimapp->openVSim(filename.toStdString());
	emit sOpenFile(filename.toStdString());
}

void MainWindow::actionSave()
{
	qDebug("save");
}

void MainWindow::actionSaveAs()
{
	qDebug("saveas");
	QString filename = QFileDialog::getSaveFileName(this, "Save VSim", "", "osg ascii file (*.osgt);;osg binary file (*.osgb)");
	if (filename == "") {
		qDebug() << "saveas cancel";
		return;
	}
	qDebug() << "saving as - " << filename;
	// 
	//m_vsimapp->saveVSim(filename.toStdString());
	emit sSaveFile(filename.toStdString());
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
	//m_vsimapp->importModel(filename.toStdString());
	emit sImportModel(filename.toStdString());
}

void MainWindow::narListForward()
{
	qDebug("Forward");
}

void MainWindow::narListAdd()
{
	qDebug("Add");
}

void MainWindow::narListDelete()
{
	qDebug("Delete");
}

void MainWindow::narListPause()
{
	qDebug("Pause");
}

void MainWindow::narListOpen()
{
	qDebug("Open");
}

void MainWindow::narListInfo()
{
	qDebug("Info");
}