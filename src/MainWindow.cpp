#include <osgViewer/Viewer>
#include <iostream>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDir>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "MainWindow.h"

#include "ui_MainWindow.h"

#include "OSGViewerWidget.h"
#include <osgUtil/Optimizer>
#include "TimeSlider.h"
#include "ModelOutliner.h"
#include "StatsWindow.h"
#include "HistoryWindow.h"
#include "FutureDialog.h"
#include "SimpleWorker.h"

#include "narrative/NarrativeGroup.h"
#include "narrative/NarrativePlayer.h"
#include "narrative/NarrativeControl.h"

#include "resources/EResourceGroup.h"
#include "resources/ERControl.h"
#include "resources/ERDisplay.h"
#include "resources/ERControl.h"
#include "resources/ERFilterArea.h"
#include "resources/ERScrollItem.h"

#include "Canvas/CanvasContainer.h"
#include "Canvas/CanvasEditor.h"

#include "VSimApp.h"
#include "VSimRoot.h"
#include "OSGYearModel.h"
#include "ModelInformationDialog.h"
#include "Model.h"
#include "ModelGroup.h"
//#include "GroupCommands.h"
#include "NavigationControl.h"
#include "AboutDialog.h"
#include "CoordinateWidget.h"
#include "WidgetStack.h"
#include "Core/Util.h"

#include "FileUtil.h"
#include <fstream>

#include "types_generated.h"
#include "settings_generated.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	// initialize the Qt Designer stuff
	ui->setupUi(this);

	// window stuff
	//setMinimumSize(800, 600);
	resize(1280, 800);
	setWindowTitle("VSim");
	setAcceptDrops(true);

	// osg viewer widget
	m_osg_widget = new OSGViewerWidget(ui->root);
	ui->rootLayout->addWidget(m_osg_widget, 0, 0);

	m_canvas = new CanvasEditor(ui->root);
	m_canvas->setObjectName("canvas");
	ui->rootLayout->addWidget(m_canvas, 0, 0);

	QMainWindow *canvas_window = m_canvas->internalWindow();
	canvas_window->setParent(ui->root);

	m_fade_canvas = new CanvasContainer();
	m_fade_canvas->setObjectName("fadeCanvas");
	ui->rootLayout->addWidget(m_fade_canvas, 0, 0);
	m_fade_canvas->lower();
	m_fade_canvas->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_fade_canvas->setStyleSheet("#canvas{background:rgba(0, 0, 0, 0);}");

	// splitter on top of osg viewer
	// mask allows events to get to the canvas
	//ui->mainSplitter->setParent(m_osg_widget);
	//ui->rootLayout->addWidget(ui->mainSplitter, 0, 0);

	ui->mainSplitter->setMouseTracking(true);

	// splitter mask
	connect(ui->mainSplitter, &QSplitter::splitterMoved, this, &MainWindow::updatePositions);

	// er display
	m_er_display = new ERDisplay(ui->root);
	//m_er_display->setGeometry(10, 10, 265, 251);
	m_er_display->setObjectName("erDisplay");
	m_er_display->hide();

	// er filter widget
	//QWidget *filter_area_padding_layout = new QGridLayout();
	//middle_layout->addLayout(filter_area_padding_layout, 0, 0);
	m_er_filter_area = new ERFilterArea(ui->root);
	m_er_filter_area->setObjectName("erFilterArea");
	m_er_filter_area->hide();

	// layering
	canvas_window->lower();
	m_canvas->lower();
	m_fade_canvas->lower();
	m_osg_widget->lower();

	// layering
	//m_widget_stack->push(m_osg_widget);
	//m_widget_stack->push(m_fade_canvas);
	//m_widget_stack->push(m_canvas);
	//m_widget_stack->push(ui->mainSplitter);

	// vsimapp file stuff
	connect(ui->actionNew, &QAction::triggered, this, &MainWindow::actionNew);
	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::actionOpen);
	connect(ui->actionSave, &QAction::triggered, this, &MainWindow::actionSave);
	connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::actionSaveAs);
	connect(ui->actionImport_Model, &QAction::triggered, this, &MainWindow::actionImportModel);
	connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
	connect(ui->actionImport_Narratives, &QAction::triggered, this, &MainWindow::actionImportNarratives);
	connect(ui->actionExport_Narratives, &QAction::triggered, this, &MainWindow::actionExportNarratives);
	connect(ui->actionImport_Resources, &QAction::triggered, this, &MainWindow::actionImportERs);
	connect(ui->actionExport_Resources, &QAction::triggered, this, &MainWindow::actionExportERs);

	connect(ui->actionOSG_Debug, &QAction::triggered, this, [this]() {
		m_app->getRoot()->models()->debugScene();
	});
	connect(ui->actionRoot_Debug, &QAction::triggered, this, [this]() {
		m_app->getRoot()->debug();
	});
	connect(ui->actionCamera_Debug, &QAction::triggered, this, &MainWindow::sDebugCamera);
	connect(ui->actionControl_Debug, &QAction::triggered, this, &MainWindow::sDebugControl);
	connect(ui->actionReload_Style, &QAction::triggered, this, &MainWindow::reloadStyle);
	reloadStyle();
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
	connect(ui->actionOptimize_Scene, &QAction::triggered, this, [this]() {
		auto *root = m_app->getRoot()->models()->sceneRoot();
		osgUtil::Optimizer optimizer;
		optimizer.optimize(root);
	});
	connect(ui->actionFont_Color_Styles, &QAction::triggered, this, &MainWindow::sEditStyleSettings);
	connect(ui->actionModel_Information, &QAction::triggered, this, &MainWindow::execModelInformation);

	// player


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

	// about
	connect(ui->actionAbout_VSim, &QAction::triggered, this, [this]() {
		AboutDialog *dlg = new AboutDialog(this);
		dlg->exec();
	});

	// history
	m_history_window = new HistoryWindow(this);
	m_history_window->resize(250, 500);
	connect(ui->actionHistory, &QAction::triggered, this, [this]() {
		m_history_window->setVisible(!m_history_window->isVisible());
	});

	// coordinate
	m_coordinate_widget = new CoordinateWidget(this);
	ui->statusbar->addPermanentWidget(m_coordinate_widget);

	QAction *a_test = new QAction("Debug Menu", this);
	a_test->setShortcut(QKeySequence(Qt::Key_F11));
	addAction(a_test);
	connect(a_test, &QAction::triggered, this, [this]() {
		ui->menubar->addAction(ui->menuTest->menuAction());
	});
	ui->menubar->removeAction(ui->menuTest->menuAction());

	QAction *box = new QAction("Scroll Box Thing", this);
	ui->menuTest->addAction(box);
	box->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	connect(box, &QAction::triggered, this, [this]() {
		auto *fsb = new FastScrollBox(this);
		fsb->insertItems({ {0, new ERScrollItem} });
		ui->bottomBar->ui.horizontalLayout->addWidget(fsb);
	});

	m_stats_window = new StatsWindow(this);
	QAction *stats = new QAction("Stats", this);
	ui->menuTest->addAction(stats);
	stats->setShortcut(QKeySequence(Qt::Key_F10));
	connect(stats, &QAction::triggered, this, [this]() {
		m_stats_window->show();
	});
	QTimer *stats_timer = new QTimer(this);
	stats_timer->setInterval(100);
	stats_timer->start();
	connect(stats_timer, &QTimer::timeout, this, [this]() {
		// stats timer update
		m_stats_window->ui.frame_time->setText(QString::number(m_osg_widget->getFrameTime()));
		m_stats_window->ui.full_frame_time->setText(QString::number(m_osg_widget->getFullFrameTime()));
		m_stats_window->ui.time_between->setText(QString::number(m_osg_widget->getTimeBetween()));
	});
}

void MainWindow::setApp(VSimApp * vsim)
{
	m_app = vsim;

	m_time_slider->setTimeManager(m_app->timeManager());

	QUndoStack *stack = m_app->getUndoStack();
	QAction *undo_action = stack->createUndoAction(this, tr("&Undo"));
	QAction *redo_action = stack->createRedoAction(this, tr("&Redo"));
	undo_action->setShortcuts(QKeySequence::Undo);
	redo_action->setShortcuts(QKeySequence::Redo);
	ui->menuEdit->addAction(undo_action);
	ui->menuEdit->addAction(redo_action);
	m_history_window->setStack(stack);
	connect(undo_action, &QAction::triggered, this, [this, stack]() {
		int index = stack->index();
		if (index < 0 || index >= stack->count()) return;
		QString text = stack->text(index);
		ui->statusbar->showMessage("Undo " + text);
		qInfo() << "undo" << text;
	});
	connect(redo_action, &QAction::triggered, this, [this, stack]() {
		int prev = stack->index() - 1;
		if (prev < 0 || prev >= stack->count()) return;
		QString text = stack->text(prev);
		ui->statusbar->showMessage("Redo " + text);
		qInfo() << "redo" << text;
	});

	// coordinate widget on every frame
	connect(m_app, &VSimApp::sTick, this,
		[this]() {
		auto mat = m_app->getCameraMatrix();
		auto pos = mat.getTrans();
		double y, p, r;
		Util::matToYPR(mat, &y, &p, &r);
		m_coordinate_widget->setCoordinate(QVector3D(pos.x(), pos.y(), pos.z()),
			QVector3D(Util::deg(y), Util::deg(p), Util::deg(r)));
	});

	m_outliner->header()->resizeSection(0, 200);
	m_outliner->resize(505, 600);
	m_outliner->expandAll();

	connect(m_app, &VSimApp::sReset, this, &MainWindow::onReset);
	connect(this, &MainWindow::sDebugCamera, m_app, &VSimApp::debugCamera);

	QMenu *nmenu = ui->menuNavigation;
	auto *nav = m_app->navigationControl();

	// init navigation menu
	nmenu->setTitle("Navigation");
	nmenu->addSeparator()->setText("Navigation Mode");
	nmenu->addAction(nav->a_first_person);
	nmenu->addAction(nav->a_flight);
	nmenu->addAction(nav->a_object);
	nmenu->addSeparator();
	nmenu->addAction(nav->a_freeze);
	nmenu->addAction(nav->a_home);
	nmenu->addSeparator();
	nmenu->addAction(nav->a_gravity);
	nmenu->addAction(nav->a_collisions);
	nmenu->addSeparator();
	nmenu->addAction(nav->a_speed_up);
	nmenu->addAction(nav->a_slow_down);

	// init render menu
	QMenu *rmenu = ui->menuRender;
	rmenu->setTitle("Render");
	rmenu->addAction(nav->a_lighting);
	rmenu->addAction(nav->a_backface_culling);
	rmenu->addAction(nav->a_texturing);
	rmenu->addAction(nav->a_stats);
	rmenu->addSeparator();
	rmenu->addAction(nav->a_cycle_mode);
	rmenu->addActions(nav->m_mode_group->actions());
}

void MainWindow::onReset()
{
	updatePositions();
	m_outliner->setModelGroup(m_app->getRoot()->models());
	m_outliner->expandAll();
}

void MainWindow::reloadStyle()
{
	qDebug() << "reload style";
	QFile file("assets/style.qss");
	file.open(QFile::ReadOnly);
	QString style = QLatin1String(file.readAll());
	setStyleSheet(style);

	QFile file2("assets/darkstyle.qss");
	file2.open(QFile::ReadOnly);
	QString dark_style = QLatin1String(file2.readAll());
	m_canvas->internalWindow()->setStyleSheet(dark_style);
}

OSGViewerWidget * MainWindow::getViewerWidget() const
{
	return m_osg_widget;
}

CanvasEditor * MainWindow::canvas() const
{
	return m_canvas;
}

CanvasContainer * MainWindow::fadeCanvas() const
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
		//emit sOpenFile(text.toStdString());
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

	// place the toolbar where the middle spacer is
	QWidget *middle = ui->middleSpacer;
	auto *canvas_window = m_canvas->internalWindow();
	//qDebug() << "middle geom" << middle->geometry();
	//qDebug() << "middle x" << middle->x() << middle->y();
	//canvas_window->setGeometry(middle->x(), middle->y(),
	//	middle->width(), middle->height());
	canvas_window->setGeometry(middle->geometry());

	// Update the splitter mask
	QSplitter *splitter = ui->mainSplitter;
	QRegion full = QRegion(0, 0, splitter->width(), splitter->height());
	QRegion center = QRegion(middle->geometry());
	QRegion toolbar;

	//auto *canvas_window = m_canvas->internalWindow();
	//canvas_window->isVisible();
	//auto r = canvas_window->mask();
	//r.map
	//if (m_canvas->internalWindow());

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
		m_app->setFileName("");
		m_app->initWithVSim();
	}
}
void MainWindow::actionOpen()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open .vsim",
		m_app->getCurrentDirectory().c_str(),
		"VSim files (*.vsim);;"
		"Model files (*.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds;*.dae);;"
		"All types (*.*)");
	execOpen(filename);
}

void MainWindow::execOpen(const QString & filename)
{
	if (filename == "") return;
	qInfo() << "opening - " << filename;
	QFileInfo file_info(filename);
	bool read_vsim = (file_info.suffix() == "vsim");

	FutureDialog dlg;
	dlg.setText("Loading " + filename);
	dlg.setWindowTitle("Open");

	// run in thread
	if (read_vsim) {

		VSimRoot *root_buf;

		SimpleWorker w;
		dlg.watchWorker(&w);
		// move the root over, this is needed because QTextDocument
		// allocates QObjects and causes crash
		// edit: moving the root is a pain so try to avoid it
		//root->moveAllToThread(w.thread());

		w.setFunc([filename, &root_buf, &w]() -> bool {
			root_buf = new VSimRoot(w.slave());
			bool ok = FileUtil::readVSimFile(filename.toStdString(), root_buf);
			return ok;
		});
		w.start();
		dlg.exec(); // spins gui
		bool ok = w.result(); // spins

		if (dlg.canceled()) {
			return;
		}
		else if (ok) {
			m_app->setFileName(filename.toStdString());
			m_app->initWithVSim(root_buf);
		}
		else {
			QMessageBox::warning(this, "Load Error", "Failed to load model " + filename);
		}
	}
	else {
		std::unique_ptr<VSimRoot> root_buf;
		// read some osg node, import it
		osg::ref_ptr<osg::Node> loaded_model;
		auto import_future = QtConcurrent::run([&]() -> bool {
			loaded_model = osgDB::readNodeFile(filename.toStdString());
			return (loaded_model != nullptr);
		});

		dlg.spin(import_future);
		bool result = import_future.result();

		if (dlg.canceled()) {
			return;
		}
		else if (result) {
			root_buf = std::make_unique<VSimRoot>();
			root_buf->models()->addNode(loaded_model, filename.toStdString());
			m_app->setFileName("");
			m_app->initWithVSim(root_buf.get());
		}
		else {
			QMessageBox::warning(this, "Load Error", "Failed to load model " + filename);
		}
	}
}

void MainWindow::actionSave()
{
	QString filename = m_app->getFileName().c_str();
	QFileInfo info(filename);
	if (m_app->getFileName() == "" || info.suffix() != "vsim") {
		actionSaveAs();
		return;
	}
	execSave(filename);
}

void MainWindow::actionSaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save VSim",
		m_app->getFileName().c_str(),
		"VSim file (*.vsim);;");
	if (filename == "") {
		return;
	}

	execSave(filename);
}

void MainWindow::execSave(const QString & filename)
{
	std::string path = filename.toStdString();

	QFuture<bool> future;
	FutureDialog dlg;
	dlg.setText("Saving " + filename);
	dlg.setWindowTitle("Save");
	dlg.setCancellable(false);

	VSimRoot *root = m_app->getRoot();

	SimpleWorker w;
	dlg.watchWorker(&w);
	// move the root over, this is needed because QTextDocument
	// allocates QObjects and causes crash
	//root->moveAllToThread(w.thread());
	w.setFunc([path, root]() -> bool {
		//VSimRoot root_copy;
		//root_copy.copy(root);
		bool ok = FileUtil::writeVSimFile(path, root);
		// move the root back to the main thread
		//root->moveAllToThread(QApplication::instance()->thread());
		return ok;
	});
	w.start();
	dlg.exec();

	bool ok = w.result();

	if (!ok) {
		QMessageBox::warning(this, "Save Error", "Error saving to file " + filename);
	}
}

void MainWindow::actionImportModel()
{
	QString filename = QFileDialog::getOpenFileName(this, "Import Model",
		m_app->getLastDiretory().c_str(),
		"Model files (*.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds;*.dae);;"
		"All types (*.*)");
	if (filename == "") {
		return;
	}
	qInfo() << "importing - " << filename;

	QFuture<osg::Node*> future;
	FutureDialog dlg;
	dlg.setFuture(future);
	dlg.setText("Loading " + filename);
	dlg.setWindowTitle("Open");

	osg::ref_ptr<osg::Node> loadedModel;

	future = QtConcurrent::run(&osgDB::readNodeFile, filename.toStdString());
	dlg.exec(); // spin until done loading
	loadedModel = future.result();

	if (dlg.canceled()) {
	}
	else if (loadedModel) {
		m_app->getRoot()->models()->addNode(loadedModel, filename.toStdString());
		m_app->navigationControl()->a_home->trigger(); // reset camera
		m_app->setLastDirectory(filename.toStdString(), true);
	}
	else {
		qWarning() << "Error importing" << filename;
		QMessageBox::warning(this, "Import Error", "Error loading file " + filename);
	}
}

void MainWindow::actionImportNarratives()
{
	// Open dialog
	qInfo("Importing narratives");
	QStringList list = QFileDialog::getOpenFileNames(this, "Import Narratives",
		m_app->getLastDiretory().c_str(), "Narrative files (*.nar);;All types (*.*)");
	if (list.empty()) {
		qInfo() << "import cancel";
		return;
	}
	QStringList error_list;
	int count = 0;
	NarrativeGroup group;
	for (const QString &filename : list) {
		if (filename.isEmpty()) continue;

		m_app->setLastDirectory(filename.toStdString(), true);

		std::ifstream in(filename.toStdString(), std::ios::binary);
		if (in.good()) {
			bool ok = FileUtil::importNarrativesStream(in, &group);
			if (ok) {
				count++;
				continue;
			}
		}
		// error case
		error_list.append(filename);
	}

	m_app->narrativeControl()->mergeNarratives(&group);

	if (!error_list.empty()) {
		QString s;
		for (const auto &s : error_list);
		QMessageBox::warning(this, "Import Narratives", "Error importing narratives from " + error_list.join(", "));;
	}
}

void MainWindow::actionExportNarratives()
{
	// Open dialog
	qInfo("Exporting narratives");
	QString filename = QFileDialog::getSaveFileName(this, "Export Narratives",
		m_app->getLastDiretory().c_str(), "Narrative files (*.nar);;All types (*.*)");
	if (filename == "") {
		qInfo() << "export cancel";
		return;
	}
	std::ofstream out(filename.toStdString(), std::ios::binary);
	if (out.good()) {
		bool ok = FileUtil::exportNarrativesStream(out, m_app->getRoot()->narratives(),
			m_app->narrativeControl()->getSelectedNarratives());
		if (ok) return;
	}
	QMessageBox::warning(this, "Export Error", "Error exporting narratives to " + filename);
}

void MainWindow::actionImportERs()
{
	qInfo("Importing resources");
	QString path = QFileDialog::getOpenFileName(this, "Import Resources",
		m_app->getLastDiretory().c_str(), "Narrative files (*.ere);;All types (*.*)");
	if (path.isEmpty()) {
		qInfo() << "import cancel";
		return;
	}

	m_app->setLastDirectory(path.toStdString(), true);

	std::ifstream in(path.toStdString(), std::ios::binary);
	if (in.good()) {
		EResourceGroup g;
		bool ok = FileUtil::importEResources(in, &g);
		m_app->erControl()->mergeERs(&g);
		return;
	}
	QMessageBox::warning(this, "Import Narratives",
		"Error importing narratives from " + path);
}

void MainWindow::actionExportERs()
{
	// Open dialog
	qInfo("Exporting resources");
	QString filename = QFileDialog::getSaveFileName(this, "Export Resources",
		m_app->getLastDiretory().c_str(), "Resources (*.ere);;All types (*.*)");
	if (filename == "") {
		qInfo() << "export cancel";
		return;
	}
	std::ofstream out(filename.toStdString(), std::ios::binary);
	if (out.good()) {
		bool ok = FileUtil::exportEResources(out, m_app->getRoot()->resources(),
			m_app->erControl()->getCombinedSelection());
		if (ok) return;
	}
	QMessageBox::warning(this, "Export Error", "Error exporting resources to " + filename);
}

void MainWindow::execModelInformation()
{
	// get model information
	auto *settings = m_app->getRoot()->settings();
	auto *info = settings->model_information.get(); // possibly missing

	ModelInformationDialog dlg(info);
	int result = dlg.exec();
	if (result == QDialog::Accepted) {
		auto new_data = dlg.getData();

		settings->model_information.reset(
			new VSim::FlatBuffers::ModelInformationT(
				new_data
			));
	}
}
