#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers> // stats handler
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/StateSetManipulator>

#include <osg/io_utils>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <QObject>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QAction>
#include <QFileDialog>
#include <QUndoStack>
#include <QStatusBar>

#include "VSimApp.h"
#include "Util.h"
#include "deprecated/narrative/Narrative.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeCanvas.h"
#include "OSGViewerWidget.h"
#include "MainWindow.h"
#include "ModelOutliner.h"
#include "TimeSlider.h"
#include "narrative/NarrativeControl.h"
#include "narrative/NarrativePlayer.h"
#include "resources/ERControl.h"
#include "VSimRoot.h"
#include "ModelTableModel.h"
#include "MainWindowTopBar.h"
#include "NavigationControl.h"

#include <QMenuBar>

VSimApp::VSimApp(MainWindow* window)
	: m_window(window),
	m_filename(""),
	m_root(new VSimRoot),
	m_model_table_model(new ModelTableModel(this))
{
	// undo stack
	m_undo_stack = new QUndoStack(this);
	m_undo_stack->setUndoLimit(50);

	// timers
	m_timer = new QTimer(this);
	m_timer->setInterval(0);
	m_dt_timer = new QElapsedTimer;
	m_timer->start();
	m_dt_timer->start();
	connect(m_timer, &QTimer::timeout, this, [this]() {
		double dt = m_dt_timer->nsecsElapsed() / 1.0e9;
		m_dt_timer->restart();
		update(dt);
	});

	m_narrative_control = new NarrativeControl(this, m_window, this);
	m_er_control = new ERControl(this, m_window, m_root->resources(), this);
	m_navigation_control = new NavigationControl(this, m_window->getViewerWidget(), m_window->navigationMenu(), this);

	// thumbnails
	m_thumbnail_size = QSize(288, 162);
	m_render_canvas = new NarrativeCanvas;
	m_render_canvas->resize(m_thumbnail_size);
	m_render_canvas->move(0, 0);
	m_render_canvas->setSlide(nullptr);
	m_render_canvas->setEditable(false);

	m_render_view = new OSGViewerWidget;
	m_render_view->resize(m_thumbnail_size);
	m_render_view->move(0, 0);
	m_render_view->setCameraFrozen(true);

	// Narrative player
	m_narrative_player = new NarrativePlayer(this, m_narrative_control, m_window->topBar(), this);

	//connect(window->topBar()->ui.play_2, &QPushButton::pressed, m_narrative_player, &NarrativePlayer::play);
	//connect(window->topBar()->ui.pause_2, &QPushButton::pressed, m_narrative_player, &NarrativePlayer::stop);
	//connect(m_narrative_player, &NarrativePlayer::updateCamera, m_window->getViewerWidget(), &OSGViewerWidget::setCameraMatrix);
	//connect(this, &VSimApp::tick, m_narrative_player, &NarrativePlayer::update);
	//connect(m_narrative_player, &NarrativePlayer::enableNavigation, window->getViewerWidget(), &OSGViewerWidget::enableNavigation);
	connect(this, &VSimApp::sTick, window->getViewerWidget(), static_cast<void(OSGViewerWidget::*)()>(&OSGViewerWidget::update));

	m_camera_timer = new QTimer(this);
	connect(m_camera_timer, &QTimer::timeout, this, [this]() {
		setCameraMatrix(m_camera_target);
	});

	initWithVSim(m_root);
}

void VSimApp::setWindow(MainWindow *)
{

}

VSimApp::State VSimApp::state() const
{
	return m_state;
}

void VSimApp::setState(State s)
{
	if (s == m_state) return;
	State old = s;
	m_state = s;
	qDebug() << "state change" << StateStrings[s];
	emit sStateChanged(old, s);
}

bool VSimApp::isPlaying() const
{
	return isPlaying(m_state);
}

bool VSimApp::isPlaying(VSimApp::State state)
{
	return state == PLAY_WAIT_CLICK
		|| state == PLAY_WAIT_TIME
		|| state == PLAY_TRANSITION
		|| state == PLAY_END
		|| state == PLAY_FLYING;
}

bool VSimApp::isFlying() const
{
	return m_state == EDIT_FLYING
		|| m_state == PLAY_FLYING;
}

void VSimApp::update(float dt_sec)
{
	//emit sTick(dt_sec);

	// Smooth camera updates
	if (cameraMoving()) {
		float t = (1 - m_camera_timer->remainingTime() / (float)m_camera_timer->interval());
		setCameraMatrix(Util::cameraMatrixInterp(m_camera_start, m_camera_target, Util::simpleCubic(0, 1, t)));
		//setCameraMatrix(Util::cameraMatrixInterp(m_camera_start, m_camera_target, t));

	}

	if (isFlying() ||
		m_state == VSimApp::PLAY_TRANSITION) {
		//m_navigation_control->update(dt_sec);
	}
}

bool VSimApp::initWithVSim(osg::Node *new_node)
{
	if (!new_node) {
		new_node = new VSimRoot();
	}
	VSimRoot *root = dynamic_cast<VSimRoot*>(new_node);
	if (root == nullptr) {
		// create a new VSimRoot, convert from old format
		root = new VSimRoot(new_node->asGroup());
	}
	else {
		qInfo() << "Root is a VSimRoot";
	}
	root->postLoad();

	emit sAboutToReset();

	// move all of the gui stuff over to the new root
	m_window->getViewerWidget()->mainView()->setSceneData(root->models()); // ideally this would be only models, but its easy to mess things up
	m_model_table_model->setGroup(root->models());
	m_narrative_control->load(root->narratives());
	m_er_control->load(root->resources());
	m_window->timeSlider()->setGroup(root->models());
	m_render_view->mainView()->setSceneData(root->models());
	m_render_canvas->setSlide(nullptr);

	m_undo_stack->clear();
	m_window->m_osg_widget->reset();

	// dereference the old root, apply the new one
	m_root = root;

	emit sReset();
	return true;
}


void VSimApp::addModel(osg::Node *node, const std::string &name)
{
	m_root->models()->addChild(node);
	node->setName(name);
	m_window->m_osg_widget->reset();
}

bool VSimApp::importModel(const std::string& filename)
{
	osg::ref_ptr<osg::Node> loadedModel(NULL);
	// TODO: special import for vsim files

	// otherwise
	loadedModel = osgDB::readNodeFile(filename);
	if (!loadedModel.get()) {
		qWarning() << "Error importing" << filename.c_str();
		QMessageBox::warning(m_window, "Import Error", "Error loading file " + QString::fromStdString(filename));
		return false;
	}
	addModel(loadedModel, Util::getFilename(filename));
	return true;
}

bool VSimApp::openVSim(const std::string & filename)
{
	std::cout << "open vsim: " << filename.c_str() << "\n";
	osg::ref_ptr<osg::Node> loadedModel;
	//osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename);
	//osgDB::Options* options = new osgDB::Options;
	//options->setPluginStringData("fileType", "Ascii");

	bool init_success = false;

	// if .vsim, use osgb, TODO: our own readerwriter?
	std::string ext = Util::getExtension(filename);
	if (ext == "vsim") {
		qInfo() << "loading vsim";
		std::ifstream ifs;
		ifs.open(filename.c_str(), std::ios::binary);
		if (!ifs.good()) {
			QMessageBox::warning(m_window, "Load Error", "Error opening file " + QString::fromStdString(filename));
			return false;
		}

		osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
		if (!rw) {
			QMessageBox::warning(m_window, "Load Error", "Error creating osgb reader " + QString::fromStdString(filename));
			return false;
		}
		osgDB::ReaderWriter::ReadResult result = rw->readNode(ifs);
		if (result.success()) {
			loadedModel = result.takeNode();
		}
		else {
			QMessageBox::warning(m_window, "Load Error", "Error converting file to osg nodes " + QString::fromStdString(filename));
			return false;
		}
		init_success = initWithVSim(loadedModel);
	}
	else if (ext == "osgb" || ext == "osgt") { // osgb or osgt
		qInfo() << "loading osgt/osgb";
		loadedModel = osgDB::readNodeFile(filename);
		if (!loadedModel) {
			QMessageBox::warning(m_window, "Load Error", "Error opening file " + QString::fromStdString(filename));
			return false;
		}
		init_success = initWithVSim(loadedModel);
	}
	else {
		qInfo() << "loading a non osg model";
		loadedModel = osgDB::readNodeFile(filename);
		if (!loadedModel) {
			QMessageBox::warning(m_window, "Load Error", "Failed to load model " + QString::fromStdString(filename));
			return false;
		}
		initWithVSim();
		addModel(loadedModel, Util::getFilename(filename));
		init_success = true;
	}

	if (!init_success) {
		QMessageBox::warning(m_window, "Load Error", "Model init failed " + QString::fromStdString(filename));
		return false;
	}
	setFileName(filename);
	return true;
}

bool VSimApp::saveVSim(const std::string& filename)
{
	m_root->preSave();
	// if vsim, then use osgb
	std::string ext = Util::getExtension(filename);
	if (ext == "vsim") {
		qInfo() << "saving vsim";
		std::ofstream ofs;
		ofs.open(filename.c_str(), std::ios::binary);
		if (!ofs.good()) {
			QMessageBox::warning(m_window, "Save Error", "Error opening file for writing " + QString::fromStdString(filename));
			return false;
		}
		osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
		if (!rw) {
			QMessageBox::warning(m_window, "Save Error", "Error creating osgb writer " + QString::fromStdString(filename));
			return false;
		}
		//// this is how you do ascii (from robertosfield on github)
		//osgDB::Options* options = new osgDB::Options;
		//options->setPluginStringData("fileType", "Ascii");
		osgDB::ReaderWriter::WriteResult result = rw->writeNode(*m_root, ofs, new osgDB::Options("WriteImageHint=IncludeData"));

		if (result.success()) {
		}
		else {
			QMessageBox::warning(m_window, "Save Error", "Error writing osg nodes " + QString::fromStdString(filename));
			return false;
		}
	}
	// otherwise use osgb/osgt, let osg decide
	else {
		bool success = osgDB::writeNodeFile(*m_root, filename, new osgDB::Options("WriteImageHint=IncludeData"));
		if (!success) {
			QMessageBox::warning(m_window, "Save Error", "Error saving to file " + QString::fromStdString(filename));
			return false;
		}
	}
	setFileName(filename);
	return true;
}

bool VSimApp::saveCurrentVSim()
{
	qInfo() << "saving current file";
	return saveVSim(m_filename);
}

bool VSimApp::exportNarratives()
{
	// figure out the selected narratives, if none are selected then error
	osg::ref_ptr<osg::Group> export_group = new NarrativeGroup;
	//NarrativeControl::SelectionLevel level = m_narrative_control->getSelectionLevel();

	auto narratives = m_narrative_control->getSelectedNarratives();
	if (narratives.empty()) {
		QMessageBox::warning(m_window, "Export Narratives Error", "No narratives selected");
		return false;
	}

	for (auto nar : narratives) {
		export_group->addChild(nar);
	}

	// Open up the save dialog
	qInfo() << "Export narratives";
	QString filename = QFileDialog::getSaveFileName(m_window, "Export Narratives",
		getCurrentDirectory(), "Narrative file (*.nar)");
	if (filename == "") {
		qInfo() << "Export narratives cancel";
		return false;
	}

	// Open the file, create the osg reader/writer
	qInfo() << "Exporting narratives" << filename;
	std::ofstream ofs;
	ofs.open(filename.toStdString(), std::ios::binary);
	if (!ofs.good()) {
		QMessageBox::warning(m_window, "Export Error", "Error opening file for writing " + filename);
		return false;
	}

	osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
	if (!rw) {
		QMessageBox::warning(m_window, "Export Error", "Error creating osgb writer " + filename);
		return false;
	}

	qInfo() << "Exporting narratives";
	osgDB::Options* options = new osgDB::Options;
	//options->setPluginStringData("fileType", "Ascii");
	options->setPluginStringData("WriteImageHint", "IncludeData");
	osgDB::ReaderWriter::WriteResult result = rw->writeNode(*export_group, ofs, options);

	if (result.success()) {
		return true;
	}
	else {
		QMessageBox::warning(m_window, "Save Error", "Error writing osg nodes " + filename);
		return false;
	}
}

bool VSimApp::importNarratives()
{
	// Open dialog
	qInfo("Importing narratives");
	QString filename = QFileDialog::getOpenFileName(m_window, "Import Narratives",
		getCurrentDirectory(), "Narrative files (*.nar);;All types (*.*)");
	if (filename == "") {
		qInfo() << "import cancel";
		return false;
	}

	osg::ref_ptr<osg::Node> loadedModel;

	// open file
	std::ifstream ifs;
	ifs.open(filename.toStdString(), std::ios::binary);
	if (!ifs.good()) {
		QMessageBox::warning(m_window, "Import Narratives Error", "Error opening file " + filename);
		return false;
	}
	// create reader/writer
	osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
	if (!rw) {
		QMessageBox::warning(m_window, "Import Narratives Error", "Error creating osgb writer " + filename);
		return false;
	}

	// perform read
	osgDB::ReaderWriter::ReadResult result = rw->readNode(ifs);
	if (result.success()) {
		loadedModel = result.takeNode();
	}
	else {
		QMessageBox::warning(m_window, "Import Narratives Error", "Error reading nodes " + filename);
		return false;
	}

	// check if a NarrativeGroup
	NarrativeGroup *group = nullptr;
	NarrativeGroup *cast = dynamic_cast<NarrativeGroup*>(loadedModel.get());
	osg::Group *cast_old = dynamic_cast<osg::Group*>(loadedModel.get());
	Narrative *nar = dynamic_cast<Narrative*>(loadedModel.get());

	if (cast) {
		group = cast;
	}
	else if (cast_old) {
		group = new NarrativeGroup(cast_old);
	}
	else if (nar) {
		qInfo() << "file is an old narrative";
		group = new NarrativeGroup;
		group->addChild(new Narrative2(nar));
	}
	else {
		qWarning() << "Error importing narratives - root node is not a NarrativeGroup or osg::Group";
		QMessageBox::warning(m_window, "Import Error", "Error loading file " + filename);
		return false;
	}

	m_narrative_control->loadNarratives(group);
	return true;
}

VSimRoot * VSimApp::getRoot() const
{
	return m_root.get();
}

QString VSimApp::getCurrentDirectory() const
{
	QFileInfo f(m_filename.c_str());
	return f.dir().path();
}

std::string VSimApp::getFileName() const
{
	return m_filename;
}

void VSimApp::setFileName(const std::string &str)
{
	m_filename = str;
	m_window->setWindowTitle("VSim - " + QString::fromStdString(str));
}

void VSimApp::setStatusMessage(const QString & message, int timeout)
{
	m_window->statusBar()->showMessage(message, timeout);
}

osg::Matrixd VSimApp::getCameraMatrix() const
{
	return m_window->getViewerWidget()->getCameraMatrix();
}

void VSimApp::setCameraMatrix(const osg::Matrixd & matrix)
{
	m_window->getViewerWidget()->setCameraMatrix(matrix);
}

void VSimApp::setCameraMatrixSmooth(const osg::Matrixd & matrix, float time)
{
	m_camera_start = getCameraMatrix();
	m_camera_target = matrix;
	m_camera_timer->setInterval(time*1000);
	m_camera_timer->setSingleShot(true);
	m_camera_timer->start();
}

bool VSimApp::cameraMoving() const
{
	return m_camera_timer->isActive();
}

void VSimApp::stopCameraMoving()
{
	m_camera_timer->stop();
}

QImage VSimApp::generateThumbnail(NarrativeSlide * slide)
{
	QImage img(m_thumbnail_size, QImage::Format_ARGB32);
	QPainter painter(&img);

	// render osg
	m_render_view->setCameraMatrix(slide->getCameraMatrix());
	m_render_view->render(&painter, QPoint(0, 0), QRect(QPoint(0, 0), m_thumbnail_size), 0);

	// render canvas
	m_render_canvas->setSlide(slide);
	m_render_canvas->render(&painter, QPoint(0, 0), QRect(QPoint(0, 0), m_thumbnail_size), QWidget::DrawChildren); // | ignore mask

	return img;
}

QUndoStack *VSimApp::getUndoStack() const
{
	return m_undo_stack;
}

void VSimApp::debugCamera()
{
	osg::Matrixd matrix = getCameraMatrix();
	osg::Vec3 trans, scale;
	osg::Quat rot, so;
	matrix.decompose(trans, rot, scale, so);

	double y, p, r;
	Util::quatToYPR(rot, &y, &p, &r);
	std::cout << "matrix " << matrix << "\ntranslation " << trans << "\nscale " << scale << "\nrotation " << rot << "\n";
	qInfo() << "ypr" << y * 180 / M_PI << p * 180 / M_PI << r * 180 / M_PI;

	std::cout << "zero everything\n";
	auto q = Util::YPRToQuat(0, 0, 0);
	std::cout << osg::Matrixd::rotate(q);

	osg::Matrix base(
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, -1, 0, 0,
		0, 0, 0, 1);
	double yaw, pitch, roll;
	Util::quatToYPR(base.getRotate(), &yaw, &pitch, &roll);
	qInfo() << yaw * M_PI/180 << pitch * M_PI / 180 << roll * M_PI / 180;

}

ModelTableModel * VSimApp::modelTable() const
{
	return m_model_table_model;
}

NarrativeControl * VSimApp::narrativeControl() const
{
	return m_narrative_control;
}

ERControl * VSimApp::erControl() const
{
	return m_er_control;
}
