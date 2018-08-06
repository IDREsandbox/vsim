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
#include "Core/Util.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "narrative/NarrativeSlide.h"
#include "Canvas/CanvasContainer.h"
#include "OSGViewerWidget.h"
#include "MainWindow.h"
#include "TimeSlider.h"
#include "narrative/NarrativeControl.h"
#include "narrative/NarrativePlayer.h"
#include "resources/ERControl.h"
#include "VSimRoot.h"
#include "MainWindowTopBar.h"
#include "NavigationControl.h"
#include "TimeManager.h"
#include "BrandingControl.h"
#include "Switch/SwitchManager.h"
#include "Model/ModelGroup.h"
#include "Model/Model.h"
#include "Model/OSGNodeWrapper.h"

#include <QMenuBar>

VSimApp::VSimApp(MainWindow* window)
	: m_window(window)
{
	m_root = std::make_unique<VSimRoot>();

	// undo stack
	m_undo_stack = new QUndoStack(this);
	m_undo_stack->setUndoLimit(50);

	// have to create this early on
	// er constructor uses this
	// FIXME: why so circular and confusing
	m_time_manager = new TimeManager(this, this);

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
	m_er_control = new ERControl(this, m_window, this);
	m_navigation_control = new NavigationControl(this, m_window->getViewerWidget(), this);
	m_branding_control = new BrandingControl(this, m_root->branding(), m_window->brandingOverlay(), this);
	m_switch_manager = new SwitchManager(this);
	connectSwitchManager();

	m_viewer = m_window->getViewerWidget();

	// Narrative player
	m_narrative_player = new NarrativePlayer(this, m_narrative_control, m_window->topBar(), this);

	m_camera_timer = new QTimer(this);
	connect(m_camera_timer, &QTimer::timeout, this, [this]() {
		setCameraMatrix(m_camera_target);
		emit sArrived();
	});
}

VSimApp::~VSimApp() {

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
	stopGoingSomewhere();
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

	// Smooth camera updates
	if (goingSomewhere()) {
		float t = (1 - m_camera_timer->remainingTime() / (float)m_camera_timer->interval());
		setCameraMatrix(Util::cameraMatrixInterp(m_camera_start, m_camera_target, Util::simpleCubic(0, 1, t)));
		//setCameraMatrix(Util::cameraMatrixInterp(m_camera_start, m_camera_target, t));

	}

	auto mat = getCameraMatrix();
	osg::Vec3 old_position = m_position;
	m_position = mat.getTrans();
	if (old_position != m_position) {
		emit sPositionChanged(m_position);
	}

	emit sTick(dt_sec);
}

bool VSimApp::initWithVSim(VSimRoot *root)
{
	std::unique_ptr<VSimRoot> generated;
	if (!root) {
		generated = std::make_unique<VSimRoot>();
		root = generated.get();
	}

	emit sAboutToReset();

	m_undo_stack->clear();

	// dereference the old root, apply the new one
	// m_root.reset(root);

	// keep the old root, just do a copy
	m_root->take(root);

	osg::Node *osg_root = m_root->models()->sceneRoot();
	m_viewer->setSceneData(osg_root);

	emit sReset();

	m_navigation_control->a_object->trigger(); // goto object mode
	return true;
}

void VSimApp::prepareSave()
{
	emit sAboutToSave();
}

VSimRoot *VSimApp::getRoot() const
{
	return m_root.get();
}

std::string VSimApp::getLastDirectory() const
{
	return m_last_directory;
}

void VSimApp::setLastDirectory(const std::string & dir, bool isFile)
{
	if (isFile) {
		QFileInfo f(dir.c_str());
		m_last_directory = f.dir().path().toStdString();
	}
	else {
		m_last_directory = dir;
	}
}

QString VSimApp::getCurrentDirectory() const
{
	if (m_current_file.isEmpty()) {
		return QDir::currentPath();
	}
	QFileInfo f(m_current_file);
	return f.dir().path();
}

QString VSimApp::getCurrentFile() const
{
	return m_current_file;
}

void VSimApp::setCurrentFile(const QString &path)
{
	QString s = path;

	QString old_dir = getCurrentDirectory();
	QString new_dir = Util::absoluteDirOf(path);

	if (!s.isEmpty()) {
		// force to be absolute path
		s = QDir(s).absolutePath();
	}
	m_current_file = s;
	m_window->setWindowTitle("VSim - " + s);

	emit sCurrentDirChanged(old_dir, new_dir);
}

void VSimApp::setStatusMessage(const QString & message, int timeout)
{
	if (m_window)
		m_window->statusBar()->showMessage(message, timeout);
}

osg::Vec3d VSimApp::getPosition() const
{
	return m_position;
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
	emit sGoingSomewhere();
}

bool VSimApp::goingSomewhere() const
{
	return m_camera_timer->isActive();
}

void VSimApp::stopGoingSomewhere()
{
	m_camera_timer->stop();
	emit sInterrupted();
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

}

//OSGYearModel *VSimApp::modelTable() const
//{
//	return m_model_table_model;
//}

NarrativeControl * VSimApp::narrativeControl() const
{
	return m_narrative_control;
}

NarrativePlayer * VSimApp::narrativePlayer() const
{
	return m_narrative_player;
}

ERControl * VSimApp::erControl() const
{
	return m_er_control;
}

NavigationControl * VSimApp::navigationControl() const
{
	return m_navigation_control;
}

TimeManager * VSimApp::timeManager() const
{
	return m_time_manager;
}

BrandingControl * VSimApp::brandingControl() const
{
	return m_branding_control;
}

OSGViewerWidget *VSimApp::viewer() const
{
	return m_viewer;
}

SwitchManager * VSimApp::switchManager() const
{
	return m_switch_manager;
}

void VSimApp::connectSwitchManager()
{
	ModelGroup *models = m_root->models();
	auto *group = models->group();
	OSGNodeWrapper *root = m_root->models()->rootWrapper();

	////added
	connect(models->rootWrapper(), &OSGNodeWrapper::sInsertedChild, this,
		[this, models] (osg::Group *g, size_t index) {
		Model *m = models->group()->child(index);
		m_switch_manager->addNodeRecursive(m->nodeRef());
	});

	//removed
	connect(models->rootWrapper(), &OSGNodeWrapper::sAboutToRemoveChild, this,
		[this, models](osg::Group *g, size_t index) {
		Model *m = models->group()->child(index);
		m_switch_manager->removeNodeRecursive(m->nodeRef());
	});

	//reset
	connect(models->rootWrapper(), &OSGNodeWrapper::sAboutToReset, [this]() {
		m_switch_manager->clear();
	});
}