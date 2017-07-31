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
#include "narrative/NarrativeInfoDialog.h"
#include "dragLabelInput.h"

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent)
{
	// initialize the Qt Designer stuff
	ui.setupUi(this);

	setMinimumSize(800, 600);
	ui.statusbar->showMessage("the best status bar", 0);
	setWindowIcon(QIcon("assets/vsim.ico"));
	setWindowTitle("VSim");
	setAcceptDrops(true);
	qDebug() << "root: " << QDir::currentPath();

	// narrative info dialog
	m_narrative_info_dialog = new NarrativeInfoDialog(this);

	// set global window settings before making viewer... so we don't have to deal with GraphicsContext stuff
	//osg::DisplaySettings::instance()->setNumMultiSamples(8);
	//qDebug() << "window aa" << osg::DisplaySettings::instance()->getNumMultiSamples();

	// osg viewer widget
	m_osg_widget = new OSGViewerWidget(ui.root);
	m_osg_widget->lower(); // move this to the back
	ui.rootLayout->addWidget(m_osg_widget, 0, 0);

	//QSurfaceFormat fmt = m_osg_widget->format();
	//fmt.setSamples(4);
	//qDebug() << "in the mainwindow" << fmt.samples();
	//m_osg_widget->setFormat(fmt);
	//glEnable(GL_MULTISAMPLE);
	//QSurfaceFormat::setDefaultFormat(fmt);


	// drag widget
	m_drag_area = new labelCanvas(ui.root);
	//m_drag_area->setGeometry(0, 0, this->width(), this->height());
	ui.rootLayout->addWidget(m_drag_area, 0, 0);

	// vsimapp file stuff
	connect(ui.actionNew, &QAction::triggered, this, &MainWindow::actionNew);
	connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::actionOpen);
	connect(ui.actionSave, &QAction::triggered, this, &MainWindow::actionSave);
	connect(ui.actionSave_As, &QAction::triggered, this, &MainWindow::actionSaveAs);
	connect(ui.actionImport_Model, &QAction::triggered, this, &MainWindow::actionImportModel);
	connect(ui.actionQuit, &QAction::triggered, this, &MainWindow::close);

	connect(ui.actionError, &QAction::triggered, this, [this] { this->ErrorDialog("heyo this is quite the error message"); });
	connect(ui.actionMessage, &QAction::triggered, this, [this] { this->MessageDialog("A beautiful message"); });
	connect(ui.actionProgress, &QAction::triggered, this, [this] { this->LoadingDialog("zzz"); });

	// camera manipulator
	connect(ui.actionFirst_Person_Navigation, &QAction::triggered, m_osg_widget,
		[this]() {qDebug() << "fp trigger"; m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_FIRST_PERSON); });
	connect(ui.actionFlight_Navigation, &QAction::triggered, m_osg_widget,
		[this]() {qDebug() << "flight trigger";  m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_FLIGHT); });
	connect(ui.actionObject_Navigation, &QAction::triggered, m_osg_widget,
		[this]() {qDebug() << "object trigger";  m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_OBJECT); });
	connect(ui.actionFreeze_Camera, &QAction::toggled, m_osg_widget,
		[this](bool freeze) {qDebug() << "freeze trigger";  m_osg_widget->setCameraFrozen(freeze); });

	// show slides or narratives
	connect(ui.topBar->ui.open, &QPushButton::clicked, this, 
		[this]() {
			qDebug() << "open";
			this->ui.topBar->showSlides();
		});
	connect(ui.topBar->ui.left_2, &QPushButton::clicked, this,
		[this]() {
			qDebug() << "goback";
			this->ui.topBar->showNarratives();
		});

	// connect narrative info
	connect(m_narrative_info_dialog, &QDialog::accepted, this, [this] { qDebug() << "narrative accept";
		auto data = this->m_narrative_info_dialog->getInfo();
		qDebug() << "Title:" << data.m_title.c_str();
		qDebug() << "Description:" << data.m_description.c_str();
		qDebug() << "Author:" << data.m_contact.c_str();
		});
	connect(m_narrative_info_dialog, &QDialog::rejected, this, [this] { qDebug() << "narrative reject"; });
	connect(m_narrative_info_dialog, &QDialog::finished, this, [this](int i) { qDebug() << "narrative finished" << i; });

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

void MainWindow::resizeEvent(QResizeEvent* event)
{
	//m_drag_area->setGeometry(0, 0, event->size().width(), event->size().height());
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
	emit sSaveCurrent();
}

void MainWindow::actionSaveAs()
{
	qDebug("saveas");
	QString filename = QFileDialog::getSaveFileName(this, "Save VSim", "", "VSim file (*.vsim);;osg ascii file (*.osgt);;osg binary file (*.osgb)");
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