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

#include "editButtons.h"
#include "narrative/NarrativeCanvas.h"

#include "resources/ERDisplay.h"
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

	// osg viewer widget
	m_osg_widget = new OSGViewerWidget(ui->root);
	ui->rootLayout->addWidget(m_osg_widget, 0, 0);

	m_canvas = new NarrativeCanvas(ui->root);
	m_canvas->setObjectName("canvas");
	ui->rootLayout->addWidget(m_canvas, 0, 0);
	m_canvas->setStyleSheet("#canvas{background:rgba(0, 0, 0, 0);}");

	m_fade_canvas = new NarrativeCanvas(ui->root);
	m_canvas->setObjectName("fadeCanvas");
	ui->rootLayout->addWidget(m_fade_canvas, 0, 0);
	m_fade_canvas->lower();
	m_fade_canvas->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_fade_canvas->setStyleSheet("#canvas{background:rgba(0, 0, 0, 0);}");

	// splitter on top of osg viewer
	// mask allows events to get to the canvas
	ui->mainSplitter->setParent(ui->root);
	ui->rootLayout->addWidget(ui->mainSplitter, 0, 0);

	ui->mainSplitter->setMouseTracking(true);

	// splitter mask
	connect(ui->mainSplitter, &QSplitter::splitterMoved, this, &MainWindow::updatePositions);

	// label buttons
	m_label_buttons = new editButtons(ui->root);

	// er display
	m_er_display = new ERDisplay(ui->root);
	m_er_display->setGeometry(10, 10, 265, 251);
	m_er_display->setObjectName("erDisplay");
	m_er_display->hide();

	// er filter widget
	//QWidget *filter_area_padding_layout = new QGridLayout();
	//middle_layout->addLayout(filter_area_padding_layout, 0, 0);
	m_er_filter_area = new ERFilterArea(ui->root);
	m_er_filter_area->setObjectName("erFilterArea");
	m_er_filter_area->hide();

	// layering
	m_canvas->lower();
	m_fade_canvas->lower();
	m_osg_widget->lower();

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
	connect(ui->actionEditor_Debug, &QAction::triggered, this, [this]() {
		qInfo() << "Editor debug";
		qInfo() << "focus object" << QApplication::focusObject();
		qInfo() << "focus widget" << QApplication::focusWidget();
		qInfo() << "updating osg viewer";
		m_osg_widget->update();
		qInfo() << "nav" << m_osg_widget->getNavigationMode();
		qInfo() << "freeze" << m_osg_widget->getCameraFrozen();
		qInfo() << "app state" << VSimApp::StateStrings[m_app->state()];
	});
	connect(ui->actionFont_Color_Styles, &QAction::triggered, this, &MainWindow::sEditStyleSettings);

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
	connect(undo_action, &QAction::triggered, this, []() {qInfo() << "undo"; });
	connect(redo_action, &QAction::triggered, this, []() {qInfo() << "redo"; });

	connect(this, &MainWindow::sOpenFile, m_app, &VSimApp::openVSim);
	connect(this, &MainWindow::sSaveFile, m_app, &VSimApp::saveVSim);
	connect(this, &MainWindow::sImportModel, m_app, &VSimApp::importModel);
	connect(this, &MainWindow::sNew, m_app, [vsim]() { vsim->initWithVSim(); });
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

NarrativeCanvas * MainWindow::canvas() const
{
	return m_canvas;
}

NarrativeCanvas * MainWindow::fadeCanvas() const
{
	return m_fade_canvas;
}

ERBar * MainWindow::erBar() const
{
	return ui->bottomBar;
}

ModelOutliner * MainWindow::outliner() const
{
	return m_outliner;
}

TimeSlider * MainWindow::timeSlider() const
{
	return m_time_slider;
}

editButtons * MainWindow::labelButtons() const
{
	return m_label_buttons;
}

QMenu * MainWindow::navigationMenu() const
{
	return ui->menuNavigation;
}

ERDisplay * MainWindow::erDisplay() const
{
	return m_er_display;
}

ERFilterArea * MainWindow::erFilterArea() const
{
	return m_er_filter_area;
}

MainWindowTopBar *MainWindow::topBar() const
{
	return ui->topBar;
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasText()) {
		qInfo() << "drag enter " << event->mimeData()->text();
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent * event)
{
	if (event->mimeData()->hasText()) {
		QString text = event->mimeData()->text();
		qInfo() << "drop file: " << text;

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
	// Place edit buttons
	int top = ui->middleSpacer->y();
	m_label_buttons->move(20, top + 30);

	// Place the tool bar area

	// Place the filter area
	// filter top (200)
	// filter height (200,290)
	// filter bottom (290)
	// space (290,300)
	// bottom top (300)
	int space = 10;
	int bottom_top = ui->bottomBar->y();
	int filter_top = bottom_top - m_er_filter_area->height() - space;
	m_er_filter_area->move(space, filter_top);

	// Place the ER popup area
	// vertical center
	int mid = m_osg_widget->height() / 2;
	int popup_top = mid - m_er_display->height() / 2;
	m_er_display->move(10, popup_top);

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
	QString filename = QFileDialog::getOpenFileName(this, "Open .vsim",
		m_app->getCurrentDirectory(),
		"VSim files (*.vsim;*.osg;*.osgt;*.osgb; );;"
		"Model files (*.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds;*.dae);;"
		"All types (*.*)");
	if (filename == "") {
		return;
	}
	qInfo() << "opening - " << filename;

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
		actionSaveAs();
		return;
	}
	emit sSaveCurrent();
}

void MainWindow::actionSaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save VSim",
		m_app->getFileName().c_str(),
		"VSim file (*.vsim;*.osgt;*.osgb);;");
	if (filename == "") {
		return;
	}
	qInfo() << "saving as - " << filename;

	//m_vsimapp->saveVSim(filename.toStdString());
	emit sSaveFile(filename.toStdString());
}

void MainWindow::actionImportModel()
{
	QString filename = QFileDialog::getOpenFileName(this, "Import Model",
		m_app->getCurrentDirectory(),
		"Model files (*.vsim;*.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds;*.dae);;"
		"All types (*.*)");
	if (filename == "") {
		return;
	}
	qInfo() << "importing - " << filename;
	//m_vsimapp->importModel(filename.toStdString());
	emit sImportModel(filename.toStdString());
}