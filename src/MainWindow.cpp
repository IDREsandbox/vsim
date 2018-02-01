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

#include "ui_MainWindow.h"

#include "OSGViewerWidget.h"
#include "TimeSlider.h"
#include "ModelOutliner.h"

#include "narrative/NarrativeCanvas.h"

#include "resources/ERDisplay.h"
#include "resources/ERFilterSortProxy.h"
#include "resources/ERScrollBox.h"
#include "resources/ERControl.h"
#include "resources/ERFilterArea.h"

#include "VSimApp.h"
#include "VSimRoot.h"
#include "ModelTableModel.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	// initialize the Qt Designer stuff
	ui->setupUi(this);

	// window stuff
	//setMinimumSize(800, 600);
	resize(1280, 720);
	setWindowTitle("VSim");
	setAcceptDrops(true);
	qDebug() << "root: " << QDir::currentPath();

	// osg viewer widget
	m_osg_widget = new OSGViewerWidget(ui->root);
	m_osg_widget->lower(); // move this to the back
	ui->rootLayout->addWidget(m_osg_widget, 0, 0);
	QGridLayout *osg_layout = new QGridLayout(m_osg_widget);
	osg_layout->setContentsMargins(QMargins()); // zero margins

	// canvas on top of osg viewer
	m_canvas = new NarrativeCanvas(m_osg_widget);
	osg_layout->addWidget(m_canvas, 0, 0);

	// splitter on top of osg viewer
	// mask allows events to get to the canvas
	ui->mainSplitter->setParent(m_osg_widget);
	osg_layout->addWidget(ui->mainSplitter, 0, 0);
	
	ui->mainSplitter->setMouseTracking(true);
	
	// splitter mask
	connect(ui->mainSplitter, &QSplitter::splitterMoved, this, &MainWindow::updatePositions);

	// er display
	m_er_display = new ERDisplay(ui->middleSpacer);
	m_er_display->setGeometry(10, 10, 265, 251);
	m_er_display->setObjectName("erDisplay");
	m_er_display->hide();

	// er filter widget
	//QWidget *filter_area_padding_layout = new QGridLayout();
	//middle_layout->addLayout(filter_area_padding_layout, 0, 0);
	m_er_filter_area = new ERFilterArea(m_osg_widget);
	m_er_filter_area->setObjectName("erFilterArea");
	m_er_filter_area->hide();

	connect(ui->filter, &QPushButton::pressed, this,
		[this]() {
		m_er_filter_area->setVisible(!m_er_filter_area->isVisible());
	});

	// vsimapp file stuff
	connect(ui->actionNew, &QAction::triggered, this, &MainWindow::actionNew);
	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::actionOpen);
	connect(ui->actionSave, &QAction::triggered, this, &MainWindow::actionSave);
	connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::actionSaveAs);
	connect(ui->actionImport_Model, &QAction::triggered, this, &MainWindow::actionImportModel);
	connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
	connect(ui->actionImport_Narratives, &QAction::triggered, this, &MainWindow::sImportNarratives);
	connect(ui->actionExport_Narratives, &QAction::triggered, this, &MainWindow::sExportNarratives);

	connect(ui->actionOSG_Debug, &QAction::triggered, this, &MainWindow::sDebugOSG);
	connect(ui->actionCamera_Debug, &QAction::triggered, this, &MainWindow::sDebugCamera);
	connect(ui->actionControl_Debug, &QAction::triggered, this, &MainWindow::sDebugControl);
	connect(ui->actionReload_Style, &QAction::triggered, this, [this]() {
		QFile File("assets/style.qss");
		File.open(QFile::ReadOnly);
		QString style = QLatin1String(File.readAll());
		setStyleSheet(style);
	});
	connect(ui->actionFont_Color_Styles, &QAction::triggered, this, &MainWindow::sEditStyleSettings);

	// camera manipulator
	connect(ui->actionFirst_Person_Navigation, &QAction::triggered, m_osg_widget,
		[this]() { m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_FIRST_PERSON); });
	connect(ui->actionFlight_Navigation, &QAction::triggered, m_osg_widget,
		[this]() { m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_FLIGHT); });
	connect(ui->actionObject_Navigation, &QAction::triggered, m_osg_widget,
		[this]() { m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_OBJECT); });
	connect(ui->actionFreeze_Camera, &QAction::toggled, m_osg_widget,
		[this](bool freeze) { m_osg_widget->setCameraFrozen(freeze); });

	// navigation actions
	m_navigation_action_group = new QActionGroup(this);
	m_navigation_action_group->setExclusive(true);
	m_action_first_person = new QAction(m_navigation_action_group);
	m_action_flight = new QAction(m_navigation_action_group);
	m_action_object = new QAction(m_navigation_action_group);
	m_action_first_person->setCheckable(true);
	m_action_flight->setCheckable(true);
	m_action_object->setCheckable(true);
	m_action_first_person->setText("First Person");
	m_action_flight->setText("Flight");
	m_action_object->setText("Object");
	m_action_first_person->setShortcut(QApplication::translate("MainWindow", "1", nullptr));
	m_action_flight->setShortcut(QApplication::translate("MainWindow", "2", nullptr));
	m_action_object->setShortcut(QApplication::translate("MainWindow", "3", nullptr));
	ui->menuView->addSeparator()->setText("Navigation Mode");
	ui->menuView->addAction(m_action_first_person);
	ui->menuView->addAction(m_action_flight);
	ui->menuView->addAction(m_action_object);
	m_action_first_person->setChecked(true);

	//out
	connect(m_navigation_action_group, &QActionGroup::triggered, this,
		[this](QAction *which) {
		if (which == m_action_first_person) {
			m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_FIRST_PERSON);
		}
		else if (which == m_action_flight) {
			m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_FLIGHT);
		}
		else if (which == m_action_object) {
			m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_OBJECT);
		}
	});
	//in
	connect(m_osg_widget, &OSGViewerWidget::sCameraFrozen, ui->actionFreeze_Camera, &QAction::setChecked);
	connect(m_osg_widget, &OSGViewerWidget::sNavigationModeChanged,
		[this](OSGViewerWidget::NavigationMode mode) {
		if (mode == OSGViewerWidget::NAVIGATION_FIRST_PERSON) {
			m_action_first_person->setChecked(true);
			//m_action_flight->setChecked(false);
			//m_action_object->setChecked(false);
		}
		else if (mode == OSGViewerWidget::NAVIGATION_FLIGHT) {
			//actionFirst_Person_Navigation->setChecked(false);
			m_action_flight->setChecked(true);
			//actionObject_Navigation->setChecked(false);
		}
		else if (mode == OSGViewerWidget::NAVIGATION_OBJECT) {
			//actionFirst_Person_Navigation->setChecked(false);
			//actionFlight_Navigation->setChecked(false);
			m_action_object->setChecked(true);
		}
	});

	connect(ui->actionReset_Camera, &QAction::triggered, m_osg_widget, &OSGViewerWidget::reset);

	// show slides or narratives
	connect(ui->topBar->ui.open, &QPushButton::clicked, this,
		[this]() {
			qDebug() << "open";
			this->ui->topBar->showSlides();
		});
	connect(ui->topBar->ui.left_2, &QPushButton::clicked, this,
		[this]() {
			qDebug() << "goback";
			this->ui->topBar->showNarratives();
		});

	// model outliner
	m_outliner = new ModelOutliner(this);
	connect(ui->actionModel_Outliner, &QAction::triggered, this, [this]() {
		m_outliner->setVisible(!m_outliner->isVisible());
		if (m_outliner->isVisible()) m_outliner->setFocus();
	});
	m_time_slider = new TimeSlider(this);
	connect(ui->actionTime_Slider, &QAction::triggered, this, [this]() {
		m_time_slider->setVisible(!m_time_slider->isVisible());
		if (m_time_slider->isVisible()) m_time_slider->setFocus();
	});
}

void MainWindow::setApp(VSimApp * vsim)
{
	m_app = vsim;

	QAction *undo_action = m_app->getUndoStack()->createUndoAction(this, tr("&Undo"));
	QAction *redo_action = m_app->getUndoStack()->createRedoAction(this, tr("&Redo"));
	undo_action->setShortcuts(QKeySequence::Undo);
	redo_action->setShortcuts(QKeySequence::Redo);
	ui->menuEdit->addAction(undo_action);
	ui->menuEdit->addAction(redo_action);
	connect(undo_action, &QAction::triggered, this, []() {qDebug() << "main undo"; });
	connect(redo_action, &QAction::triggered, this, []() {qDebug() << "main redo"; });

	connect(this, &MainWindow::sOpenFile, m_app, &VSimApp::openVSim);
	connect(this, &MainWindow::sSaveFile, m_app, &VSimApp::saveVSim);
	connect(this, &MainWindow::sImportModel, m_app, &VSimApp::importModel);
	connect(this, &MainWindow::sNew, m_app, &VSimApp::init);
	connect(this, &MainWindow::sSaveCurrent, m_app, &VSimApp::saveCurrentVSim);
	connect(this, &MainWindow::sImportNarratives, m_app, &VSimApp::importNarratives);
	connect(this, &MainWindow::sExportNarratives, m_app, &VSimApp::exportNarratives);

	outliner()->setModel(m_app->modelTable());
	outliner()->header()->resizeSection(0, 200);
	outliner()->resize(505, 600);
	outliner()->expandAll();

	connect(m_app, &VSimApp::sReset, this, &MainWindow::onReset);
	connect(this, &MainWindow::sDebugOSG, this, [this]() {
		m_app->getRoot()->debug();
		m_app->erControl()->debug();
	});
	connect(this, &MainWindow::sDebugCamera, m_app, &VSimApp::debugCamera);

}

void MainWindow::onReset()
{
	outliner()->expandAll();
	updatePositions();
}

OSGViewerWidget * MainWindow::getViewerWidget() const
{
	return m_osg_widget;
}

osgViewer::Viewer *MainWindow::getViewer() const
{
	return m_osg_widget->getViewer();
}

NarrativeCanvas * MainWindow::canvas() const
{
	return m_canvas;
}

ModelOutliner * MainWindow::outliner() const
{
	return m_outliner;
}

TimeSlider * MainWindow::timeSlider() const
{
	return m_time_slider;
}

ERScrollBox * MainWindow::erLocal() const
{
	return ui->local;
}

ERScrollBox * MainWindow::erGlobal() const
{
	return ui->global;
}

ERDisplay * MainWindow::erDisplay() const
{
	return m_er_display;
}

ERFilterArea * MainWindow::erFilterArea() const
{
	return m_er_filter_area;
}

QAbstractButton * MainWindow::newERButton() const
{
	return ui->newERButton;
}

QAbstractButton * MainWindow::deleteERButton() const
{
	return ui->deleteERButton;
}

QAbstractButton * MainWindow::editERButton() const
{
	return ui->editERButton;
}

QAbstractButton * MainWindow::filterERButton() const
{
	return ui->filter;
}

MainWindowTopBar *MainWindow::topBar() const
{
	return ui->topBar;
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

void MainWindow::resizeEvent(QResizeEvent * event)
{
	QMainWindow::resizeEvent(event);
	updatePositions();
}

void MainWindow::updatePositions()
{
	// Place the filter area
	// filter top (200)
	// filter height (200,290)
	// filter bottom (290)
	// space (290,300)
	// bottom top (300)
	int space = 10;
	int bottom_top = ui->bottomBar->y();
	int filter_top = bottom_top - m_er_filter_area->height() + space;
	m_er_filter_area->move(space, filter_top);

	// Place the ER popup areas

	// Update the splitter mask
	QSplitter *splitter = ui->mainSplitter;
	QWidget *spacer = ui->middleSpacer;
	QRegion full = QRegion(0, 0, splitter->width(), splitter->height());
	QRegion center = QRegion(spacer->geometry());
	splitter->setMask(full - center);
}

void MainWindow::actionNew()
{
	QMessageBox::StandardButton reply =
		QMessageBox::question(
			this,
			"New file",
			"Are you sure you want to create a new file?",
			QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		emit sNew();
	}
}
void MainWindow::actionOpen()
{
	qDebug("open action");
	QString filename = QFileDialog::getOpenFileName(this, "Open .vsim",
		m_app->getCurrentDirectory(),
		"VSim files (*.vsim;*.osg;*.osgt;*.osgb; );;"
		"Model files (*.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds;*.dae);;"
		"All types (*.*)");
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
	if (m_app->getFileName() == "") {
		actionSaveAs();
		return;
	}
	QFileInfo info(m_app->getFileName().c_str());

	QString ext = info.suffix();
	if (ext != "vsim" && ext != "osgt" && ext != "osgb") {
		qDebug() << "save something funny";
		actionSaveAs();
		return;
	}
	emit sSaveCurrent();
}

void MainWindow::actionSaveAs()
{
	qDebug("saveas");
	QString filename = QFileDialog::getSaveFileName(this, "Save VSim",
		m_app->getFileName().c_str(),
		"VSim file (*.vsim;*.osgt;*.osgb);;");
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
	QString filename = QFileDialog::getOpenFileName(this, "Import Model",
		m_app->getCurrentDirectory(),
		"Model files (*.vsim;*.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds;*.dae);;"
		"All types (*.*)");
	if (filename == "") {
		qDebug() << "import cancel";
		return;
	}
	qDebug() << "importing - " << filename;
	//m_vsimapp->importModel(filename.toStdString());
	emit sImportModel(filename.toStdString());
}