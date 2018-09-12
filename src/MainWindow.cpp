#include "MainWindow.h"

#include <iostream>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDir>
#include <QScreen>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>

#include "ui_MainWindow.h"

#include "OSGViewerWidget.h"
#include "TimeSlider.h"
#include "Model/ModelOutliner.h"
#include "StatsWindow.h"
#include "HistoryWindow.h"
#include "Gui/FutureDialog.h"
#include "Gui/SimpleWorker.h"
#include "Gui/PasswordDialog.h"
#include "BrandingOverlay.h"
#include "Switch/SwitchWindow.h"
#include "Switch/SwitchManager.h"

#include "narrative/NarrativeGroup.h"
#include "narrative/NarrativePlayer.h"
#include "narrative/NarrativeControl.h"
#include "narrative/Narrative.h"

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
#include "VSimSerializer.h"
#include "Model/OSGYearModel.h"
#include "Model/Model.h"
#include "Model/ModelGroup.h"
#include "Model/ModelUtil.h"
#include "Model/ModelSerializer.h"
#include "ModelInformationDialog.h"
//#include "GroupCommands.h"
#include "NavigationControl.h"
#include "AboutDialog.h"
#include "CoordinateWidget.h"
#include "BrandingControl.h"
#include "LockDialog.h"
#include "Core/Util.h"
#include "ExportDialog.h"
#include "ECategoryLegend.h"

#include <fstream>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	// initialize the Qt Designer stuff
	ui->setupUi(this);

	// window stuff
	//setMinimumSize(800, 600);
	resize(1280, 900);
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

	m_branding_overlay = new BrandingOverlay(ui->root);
	m_branding_overlay->setObjectName("branding");
	m_branding_overlay->show();
	//ui->rootLayout->addWidget(m_branding_overlay, 0, 0);
	// put the toolbar on top over everything
	//auto *branding_window = m_branding_overlay->m_editor->internalWindow();
	//branding_window->setParent(ui->root);
	//ui->rootLayout->addWidget(branding_window, 0, 0);

	// splitter on top of osg viewer
	// mask allows events to get to the canvas
	//ui->mainSplitter->setParent(m_osg_widget);
	//ui->rootLayout->addWidget(ui->mainSplitter, 0, 0);

	ui->mainSplitter->setMouseTracking(true);

	QGridLayout *middle_layout = new QGridLayout(ui->middleSpacer);

	// splitter mask
	connect(ui->mainSplitter, &QSplitter::splitterMoved, this, &MainWindow::updatePositions);

	// er display

	m_er_display = new ERDisplay(ui->root);
	//m_er_display->setGeometry(10, 10, 265, 251);
	m_er_display->setObjectName("erDisplay");
	m_er_display->hide();

	// Using layouts would be perfect. unfortunately it blocks out mouse events
	// it also sets parent, so things like hidden break it too

	//QVBoxLayout *er_display_layout = new QVBoxLayout();
	//middle_layout->addLayout(er_display_layout, 0, 0);
	//er_display_layout->addSpacing(100);
	//er_display_layout->addWidget(m_er_display, 1, Qt::AlignLeft);
	//m_er_display->setMaximumWidth(260);
	//er_display_layout->setMargin(20);
	//er_display_layout->addSpacing(100);

	// tests:
	// WA_TransparentForMouseEvents - blocks mouse events for er display
	// hidden - hidden
	//ui->middleSpacer->setAttribute(Qt::WA_TransparentForMouseEvents);
	//ui->middleSpacer->show();
	//ui->middleSpacer->setStyleSheet("background-color:rgba(255, 0, 0, 100);");


	// er filter widget
	//QWidget *filter_area_padding_layout = new QGridLayout();
	//middle_layout->addLayout(filter_area_padding_layout, 0, 0);
	m_er_filter_area = new ERFilterArea(this);
	m_er_filter_area->setWindowFlags(Qt::WindowType::Window);
	m_er_filter_area->setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);
	m_er_filter_area->setObjectName("erFilterArea");
	m_er_filter_area->setWindowTitle("Resource Filters");
	m_er_filter_area->hide();

	m_category_legend = new ECategoryLegend(ui->root);

	// layering
	m_branding_overlay->lower();
	canvas_window->lower();
	m_canvas->lower();
	m_fade_canvas->lower();
	m_osg_widget->lower();

	// vsimapp file stuff
	connect(ui->actionNew, &QAction::triggered, this, &MainWindow::actionNew);
	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::actionOpen);
	connect(ui->actionSave, &QAction::triggered, this, &MainWindow::actionSave);
	connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::actionSaveAs);
	connect(ui->action_package, &QAction::triggered, this, &MainWindow::actionPackage);
	connect(ui->actionImport_Model, &QAction::triggered, this, &MainWindow::actionImportModel);
	connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
	connect(ui->actionImport_Narratives, &QAction::triggered, this, &MainWindow::actionImportNarratives);
	connect(ui->actionExport_Narratives, &QAction::triggered, this, &MainWindow::actionExportNarratives);
	connect(ui->actionImport_Resources, &QAction::triggered, this, &MainWindow::actionImportERs);
	connect(ui->actionExport_Resources, &QAction::triggered, this, &MainWindow::actionExportERs);

	connect(ui->actionLock_Settings, &QAction::triggered, this, &MainWindow::execLockDialog);

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

	// switches
	m_switch_window = new SwitchWindow(this);
	connect(ui->switches, &QAction::triggered, this, [this]() {
		m_switch_window->setVisible(!m_switch_window->isVisible());
	});

	// coordinate
	m_coordinate_widget = new CoordinateWidget(this);
	ui->statusbar->addPermanentWidget(m_coordinate_widget);

	// debug stuff
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

	QAction *debug_frame = new QAction("Debug frame size", this);
	ui->menuTest->addAction(debug_frame);
	debug_frame->setShortcut(Qt::CTRL + Qt::Key_F1);
	connect(debug_frame, &QAction::triggered, this, [this]() {
		qInfo() << "debug frame" << "geom:" << geometry() << "frame" << frameGeometry();
	});

	m_stats_window = new StatsWindow(this);
	QAction *stats = new QAction("Stats", this);
	ui->menuTest->addAction(stats);
	stats->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F10));
	connect(stats, &QAction::triggered, this, [this]() {
		m_stats_window->show();
	});
	QTimer *stats_timer = new QTimer(this);
	stats_timer->setInterval(100);
	stats_timer->start();
	connect(stats_timer, &QTimer::timeout, this, [this]() {
		// stats timer update
		m_stats_window->clear();
		m_stats_window->ui.frame_time->setText(QString::number(m_osg_widget->getFrameTime()));
		m_stats_window->ui.full_frame_time->setText(QString::number(m_osg_widget->getFullFrameTime()));
		m_stats_window->ui.time_between->setText(QString::number(m_osg_widget->getTimeBetween()));
		m_stats_window->ui.debug_dump->setText(m_osg_widget->debugString());
		m_stats_window->addLine("ers:" + m_app->erControl()->debugString());
	});
	connect(ui->actionOSG_Debug, &QAction::triggered, this, [this]() {
		m_app->getRoot()->models()->debugScene();
	});
	connect(ui->actionRoot_Debug, &QAction::triggered, this, [this]() {
		m_app->getRoot()->debug();
	});
	connect(ui->actionCamera_Debug, &QAction::triggered, this, &MainWindow::sDebugCamera);
	connect(ui->actionControl_Debug, &QAction::triggered, this, &MainWindow::sDebugControl);
	connect(ui->actionReload_Style, &QAction::triggered, this, &MainWindow::reloadStyle);
	connect(ui->actionClear_Thumbnails, &QAction::triggered, this, [this]() {
		m_app->narrativeControl()->clearAllThumbnails();
	});
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
	// put the history button at the end
	ui->menuEdit->removeAction(ui->actionHistory);
	ui->menuEdit->addSeparator();
	ui->menuEdit->addAction(ui->actionHistory);
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

	connect(m_app, &VSimApp::sCurrentDirChanged, this,
		[this](const QString &old_dir, const QString &new_dir) {
		m_outliner->setCurrentDir(new_dir);
	});

	m_outliner->setModelGroup(m_app->getRoot()->models());
	m_outliner->resize(505, 600);

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
	nmenu->addAction(nav->a_ground_mode);
	nmenu->addAction(nav->a_collisions);
	nmenu->addSeparator();
	nmenu->addAction(nav->a_speed_up);
	nmenu->addAction(nav->a_slow_down);
	nmenu->addSeparator();
	nmenu->addAction(nav->a_reposition);

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
	rmenu->addSeparator();

	QMenu *smenu = ui->menuSettings;
	smenu->clear();
	smenu->addActions({
		nav->a_navigation_settings,
		nav->a_display_settings,
		m_canvas->a_edit_styles,
		ui->actionLock_Settings
		});

	QMenu *aa_menu = rmenu->addMenu("Anti Aliasing");
	QActionGroup *aa_group = new QActionGroup(this);
	std::vector<std::pair<const char*, int>> aa_map = {
		{"None", 0},
		{"2x", 2},
		{"4x", 4},
		{"8x", 8}
	};
	for (auto &pair : aa_map) {
		const char *text = pair.first;
		int samples = pair.second;
		QAction *aa = aa_menu->addAction(text);
		aa->setCheckable(true);
		aa_group->addAction(aa);
		if (samples == m_osg_widget->samples()) {
			aa->setChecked(true);
		}
		connect(aa, &QAction::triggered, this, [this, samples]() {
			m_osg_widget->setSamples(samples);
		});
	}

	// switches
	m_switch_window->setModel(m_app->switchManager()->listModel());

	// model menu
	QMenu *model_menu = ui->menuModel;
	model_menu->addAction(m_app->brandingControl()->a_edit);

	connect(ui->actionModel_Information, &QAction::triggered, this, &MainWindow::execModelInformation);

	connect(m_app, &VSimApp::sAboutToSave, this, [this]() {
		// gather settings
		auto *ws = &m_app->getRoot()->windowSettings();

		if (m_app->getRoot()->settingsLocked()) return;

		int x = ui->bottomBar->height();
		ws->window_width = width();
		ws->window_height = height();
		ws->has_window_size = true;
		auto sizes = ui->mainSplitter->sizes();
		ws->nbar_size = sizes.at(0);
		ws->ebar_size = sizes.at(2);
	});

	onReadOnlyChanged();
	connect(m_app->getRoot()->lockTable(), &LockTable::sLockChanged, this,
		&MainWindow::onReadOnlyChanged);
}

void MainWindow::onReset()
{
	onReadOnlyChanged();

	updatePositions();

	// extract settings
	auto *ws = &m_app->getRoot()->windowSettings();
	// best fit the rectangle
	if (ws->has_window_size) {
		resize(ws->window_width, ws->window_height);

		QScreen *screen = QGuiApplication::primaryScreen();
		QRect srect = screen->availableGeometry();
		QRect wrect(0, 0, ws->window_width, ws->window_height);

		double max_width_ratio = 1.0;
		double max_height_ratio = 1.0;

		QRect mrect(0, 0, srect.width() * max_width_ratio, srect.height() * max_height_ratio);
		mrect.moveCenter(srect.center());

		// center the rect
		wrect.moveCenter(mrect.center());
		// clip the rect
		wrect = wrect.intersected(mrect);

		int top_diff = geometry().top() - frameGeometry().top();

		// try to center based on frame size
		resize(wrect.size());
		QSize frame_size = frameGeometry().size();
		move(srect.center() - QPoint(frame_size.width() / 2, frame_size.height() / 2));

		// we could also use QWidget::saveState() and QWidget::saveGeometry()

		// Windows 10:
		// Frame top left corner when perfectly aligned: (-8,0)
		// Geometry when perfectly aligned: (0,31)
		// it's hard to make this perfect
	}

	int top = ws->nbar_size;
	int bottom = ws->nbar_size;
	int middle = ui->mainSplitter->height() - top - bottom;
	ui->mainSplitter->setSizes({ top, middle, bottom });

	// the splitter moved signal is funky
	// and the correct sizes don't get to updatePositions()
	// call again just because
	updatePositions();
}

void MainWindow::reloadStyle()
{
	qInfo() << "reload style";
	QFile file(QCoreApplication::applicationDirPath() + "/assets/style.qss");
	file.open(QFile::ReadOnly);
	QString style = QLatin1String(file.readAll());
	setStyleSheet(style);

	QFile file2(QCoreApplication::applicationDirPath() + "/assets/darkstyle.qss");
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

BrandingOverlay * MainWindow::brandingOverlay() const
{
	return m_branding_overlay;
}

ECategoryLegend * MainWindow::categoryLegend() const
{
	return m_category_legend;
}

QMenu * MainWindow::navigationMenu() const
{
	return ui->menuNavigation;
}

HistoryWindow * MainWindow::historyWindow() const
{
	return m_history_window;
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
		QString path = QUrl(text).toLocalFile();
		execOpen(path);
	}
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
	QMainWindow::resizeEvent(event);
	updatePositions();
}

void MainWindow::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::MouseButton::RightButton
		&& m_app->isFlying()
		|| m_app->state() == VSimApp::State::EDIT_ERS) {
		m_app->erControl()->closeAll();
	}
	else if (event->button() == Qt::MouseButton::LeftButton
		&&
		(m_app->state() == VSimApp::PLAY_WAIT_CLICK)
		//|| m_app->state() == VSimApp::PLAY_TRANSITION
		//|| m_app->state() == VSimApp::PLAY_WAIT_TIME
		){
		m_app->narrativePlayer()->next();
	}
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
	int bottom_top = ui->middleSpacer->height() + top;
	int filter_top = bottom_top - m_er_filter_area->height() - space;

	//m_er_filter_area->move(space, filter_top);

	// Place the ER popup area
	// vertical center
	//int mid = m_osg_widget->height() / 2;
	//int popup_top = mid - m_er_display->height() / 2;
	//m_er_display->move(10, popup_top);

	int er_margin_left = 35;
	int er_margin = 50;
	int er_width = 270;
	QRect er_rect = QRect();
	er_rect.setLeft(er_margin_left);
	er_rect.setWidth(er_width);
	er_rect.setTop(top + er_margin);
	er_rect.setBottom(bottom_top - er_margin);
	m_er_display->setGeometry(er_rect);

	// place the toolbar where the middle spacer is
	QWidget *middle = ui->middleSpacer;
	QRect midrect = middle->geometry();
	auto *canvas_window = m_canvas->internalWindow();
	canvas_window->setGeometry(midrect);

	m_branding_overlay->setGeometry(middle->geometry());

	// Update the splitter mask
	QSplitter *splitter = ui->mainSplitter;
	QRegion full = QRegion(0, 0, splitter->width(), splitter->height());
	QRegion center = QRegion(middle->geometry());
	QRegion toolbar;

	QPoint legend_margin(10, 10);
	m_category_legend->setAnchor(midrect.bottomRight() - legend_margin);

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
		m_app->setCurrentFile("");
		m_app->initWithVSim();
	}
}
void MainWindow::actionOpen()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open .vsim",
		QString(),
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
		// owned by wimple worker
		VSimRoot *root_buf;

		SimpleWorker w;
		dlg.watchWorker(&w);
		// move the root over, this is needed because QTextDocument
		// allocates QObjects and causes crash
		// edit: moving the root is a pain so try to avoid it
		//root->moveAllToThread(w.thread());

		// need new_base for model loading to work
		TypesSerializer::Params p;
		p.new_base = Util::absoluteDirOf(filename);

		w.setFunc([filename, &root_buf, &w, p]() -> bool {
			root_buf = new VSimRoot(w.slave());

			bool ok = VSimSerializer::readVSimFile(filename.toStdString(), root_buf, p);
			return ok;
		});
		w.start();
		dlg.exec(); // spins gui
		bool ok = w.result(); // spins

		if (dlg.canceled()) {
			return;
		}
		if (!ok) {
			QMessageBox::warning(this, "Load Error", "Failed to load model " + filename);
			return;
		}

		// check expiration date
		// require a password to proceed
		if (root_buf->lockTable()->isLocked()
			&& root_buf->expires()) {
			QDate expires = root_buf->expirationDate();
			QDate current = QDate::currentDate();
			if (current > expires) {
				TestPasswordDialog dlg;
				dlg.setWindowTitle("File Expired");
				dlg.setLabel("This model expired on " + expires.toString() + ". Enter password to open.");
				dlg.setLock(root_buf->lockTable());
				dlg.exec();
				if (!dlg.unlocked()) {
					QMessageBox::warning(this, "Load Error", "Failed to unlock expired model " + filename);
					return;
				}
			}
		}

		m_app->setCurrentFile(filename);
		m_app->initWithVSim(root_buf);
	}
	else {
		std::unique_ptr<VSimRoot> root_buf;
		// read some osg node, import it
		osg::ref_ptr<osg::Node> loaded_model;
		auto import_future = QtConcurrent::run([&]() -> bool {
			loaded_model = ModelSerializer::readNodeFile(filename.toStdString());
			return (loaded_model != nullptr);
		});

		dlg.spin(import_future);
		bool result = import_future.result();

		if (dlg.canceled()) {
			return;
		}
		else if (result) {
			root_buf = std::make_unique<VSimRoot>();
			auto model = std::make_shared<Model>();
			// embed or no? or dialog?
			//model->setFile(filename.toStdString(), loaded_model);
			model->setName(QUrl(filename).fileName().toStdString());
			model->embedModel(loaded_model);

			root_buf->models()->addModel(model);
			m_app->setCurrentFile("");
			m_app->initWithVSim(root_buf.get());
		}
		else {
			QMessageBox::warning(this, "Load Error", "Failed to load model " + filename);
		}
	}
}

void MainWindow::actionSave()
{
	QString filename = m_app->getCurrentFile();
	QFileInfo info(filename);
	if (filename == "" || info.suffix() != "vsim") {
		actionSaveAs();
		return;
	}
	execSave(filename);
}

void MainWindow::actionSaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save VSim",
		m_app->getCurrentFile(),
		"VSim file (*.vsim);;");
	if (filename == "") {
		return;
	}

	bool ok = execSave(filename);
	if (ok) {
		m_app->setCurrentFile(filename, true);
	}
}

bool MainWindow::execSave(const QString & filename)
{
	m_app->prepareSave();
	bool ok = execSave(filename, m_app->getRoot());
	if (!ok) {
		QMessageBox::warning(this, "Save Error",
			"Error saving to file " + filename);
	}
	return ok;
}

bool MainWindow::execSave(const QString &filename, VSimRoot *root)
{
	std::string path = filename.toStdString();

	QFuture<bool> future;
	FutureDialog dlg;
	dlg.setText("Saving " + filename);
	dlg.setWindowTitle("Save");
	dlg.setCancellable(false);

	auto p = saveParams(filename);

	SimpleWorker w;
	dlg.watchWorker(&w);
	// move the root over, this is needed because QTextDocument
	// allocates QObjects and causes crash
	//root->moveAllToThread(w.thread());
	w.setFunc([path, root, p]() -> bool {
		//VSimRoot root_copy;
		//root_copy.copy(root);
		bool ok = VSimSerializer::writeVSimFile(path, root, p);
		// move the root back to the main thread
		//root->moveAllToThread(QApplication::instance()->thread());
		return ok;
	});
	w.start();
	dlg.exec();

	bool ok = w.result();

	return ok;
}

void MainWindow::actionImportModel()
{
	ModelGroup *models = m_app->getRoot()->models();
	ModelUtil::execImportModel(models, m_app->getCurrentDirectory(), QString(), this);
}

void MainWindow::actionImportNarratives()
{
	if (m_app->getRoot()->restrictedToCurrent()) return;

	// Open dialog
	qInfo("Importing narratives");
	QStringList list = QFileDialog::getOpenFileNames(this, "Import Narratives",
		QString(), "Narrative files (*.nar);;All types (*.*)");
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
			bool ok = VSimSerializer::importNarrativesStream(in, &group);
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
	qInfo("Exporting narratives");
	ExportDialog dlg;
	dlg.setTitle("Export Narratives");
	dlg.setTypes("Narratives (*.nar);;All types (*.*)");
	int result = dlg.exec();
	if (result == QDialog::Rejected) return;

	NarrativeGroup group;
	group = *m_app->getRoot()->narratives();

	// make a copy of resources
	// apply locks
	if (dlg.lock()) {
		if (dlg.usePassword()) {
			HashLock hash = HashLock::generateHash(dlg.password().toStdString());
			for (auto nar : group) {
				nar->lockTable()->lockWithPasswordHash(hash);
			}
		}
		else {
			for (auto nar : group) {
				nar->lockTable()->lock();
			}
		}
	}

	auto selection = m_app->narrativeControl()->getSelectedNarratives();
	if (dlg.exportAll()) {
		selection.clear();
		for (size_t i = 0; i < group.size(); i++) {
			selection.insert(i);
		}
	}
	QString filename = dlg.path();

	std::ofstream out(filename.toStdString(), std::ios::binary);
	if (out.good()) {
		bool ok = VSimSerializer::exportNarrativesStream(out,
			&group,
			selection);
		if (ok) return;
	}
	QMessageBox::warning(this, "Export Error", "Error exporting narratives to " + filename);
}

void MainWindow::actionImportERs()
{
	if (m_app->getRoot()->restrictedToCurrent()) return;

	qInfo("Importing resources");
	QString path = QFileDialog::getOpenFileName(this, "Import Resources",
		QString(), "Narrative files (*.ere);;All types (*.*)");
	if (path.isEmpty()) {
		qInfo() << "import cancel";
		return;
	}

	m_app->setLastDirectory(path.toStdString(), true);

	std::ifstream in(path.toStdString(), std::ios::binary);
	if (in.good()) {
		EResourceGroup g;

		// convert relative paths to work with this file
		TypesSerializer::Params p;
		p.old_base = Util::absoluteDirOf(path);
		p.new_base = m_app->getCurrentDirectory();

		bool ok = VSimSerializer::importEResources(in, &g, p);
		m_app->erControl()->mergeERs(&g);
		return;
	}
	QMessageBox::warning(this, "Import Narratives",
		"Error importing narratives from " + path);
}

void MainWindow::actionExportERs()
{
	qInfo() << "Exporting ers";
	ExportDialog dlg;
	dlg.setTitle("Export Resources");
	dlg.setTypes("Resources (*.ere);;All types (*.*)");
	int result = dlg.exec();
	if (result == QDialog::Rejected) return;

	EResourceGroup group;
	group = *m_app->getRoot()->resources();

	// make a copy of resources
	// apply locks
	if (dlg.lock()) {
		if (dlg.usePassword()) {
			HashLock hash = HashLock::generateHash(dlg.password().toStdString());
			for (auto res : group) {
				res->lockTable()->lockWithPasswordHash(hash);
			}
		}
		else {
			for (auto res : group) {
				res->lockTable()->lock();
			}
		}
	}

	auto selection = m_app->erControl()->getCombinedSelection();
	if (dlg.exportAll()) {
		selection.clear();
		for (size_t i = 0; i < group.size(); i++) {
			selection.insert(i);
		}
	}
	QString filename = dlg.path();
	std::ofstream out(filename.toStdString(), std::ios::binary);
	if (out.good()) {
		bool ok = VSimSerializer::exportEResources(out,
			&group,
			selection,
			saveParams(filename));
		if (ok) return;
	}
	QMessageBox::warning(this, "Export Error", "Error exporting resources to " + filename);
}

void MainWindow::actionPackage()
{
	// same thing as save as, but with a lock dialog attached
	m_app->prepareSave();

	// make a copy of root
	VSimRoot root;
	root.copy(m_app->getRoot());

	LockDialog dlg;

	// fix relative paths and all that
	//root.fixRelativePaths();

	// lock stuff
	// exec lock settings
	//   exec file dialog
	//root.lockTable();
	int result = dlg.execPackage(&root);
	if (result == QDialog::Rejected) {
		return;
	}

	QString path = dlg.filePath();

	// then do the whole save thing
	bool save_ok = execSave(dlg.filePath(), &root);

	if (save_ok) {
		QStringList lock_msgs = dlg.resultsString();
		lock_msgs.push_front(QString("Successfully exported to %1").arg(path));
		QMessageBox::information(this, "Export VSim", lock_msgs.join('\n'));
	}
}

void MainWindow::execModelInformation()
{
	// get model information
	auto &info = m_app->getRoot()->modelInformation();

	bool read_only = m_app->getRoot()->lockTableConst()->isLocked();

	ModelInformationDialog dlg(&info);
	dlg.setReadOnly(read_only);
	int result = dlg.exec();
	if (result == QDialog::Rejected) return;
	if (read_only) return;

	auto new_info = dlg.getData();
	VSimRoot::copyModelInformation(info, new_info);
}

void MainWindow::execLockDialog()
{
	LockDialog dlg;
	dlg.execEdit(m_app->getRoot());

	if (dlg.lockApplied()) {
		m_app->getUndoStack()->clear();
	}
}

void MainWindow::onReadOnlyChanged()
{
	bool locked = m_app->getRoot()->lockTable()->isLocked();
	m_outliner->setReadOnly(locked);
}

TypesSerializer::Params MainWindow::saveParams(const QString &path)
{
	QString from = m_app->getCurrentDirectory();
	QString to = Util::absoluteDirOf(path);
	return {from, to};
}

//TypesSerializer::Params MainWindow::loadParams(const QString &path)
//{
//	QString to = m_app->getCurrentDirectory();
//	QString from = Util::absoluteDirOf(path);
//	return {};
//}
