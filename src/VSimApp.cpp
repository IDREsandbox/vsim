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
#include "FileUtil.h"
#include "ModelGroup.h"
#include "TimeManager.h"

#include <QMenuBar>

VSimApp::VSimApp(MainWindow* window)
	: m_window(window),
	m_filename("")
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

	// thumbnails
	m_thumbnail_size = QSize(288, 162);
	m_render_canvas = new CanvasContainer(); // FIXME: leak?
	m_render_canvas->resize(m_thumbnail_size);
	m_render_canvas->move(0, 0);
	m_render_canvas->setScene(nullptr);
	m_render_canvas->setEditable(false);

	m_render_view = new OSGViewerWidget();
	m_render_view->resize(m_thumbnail_size);
	m_render_view->move(0, 0);
	m_render_view->setCameraFrozen(true);
	m_render_view->hide();
	m_render_view->enableRendering(false);

	m_viewer = new osgViewer::CompositeViewer;
	m_main_view = m_window->getViewerWidget();
	//m_viewer->addView(m_main_view->mainView());
	m_main_view->setViewer(m_viewer);

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

	m_render_canvas->setScene(nullptr);

	m_undo_stack->clear();

	// dereference the old root, apply the new one
	// m_root.reset(root);

	// keep the old root, just do a copy
	m_root->take(root);

	osg::Node *osg_root = m_root->models()->sceneRoot();
	m_render_view->mainView()->setSceneData(osg_root);
	m_window->getViewerWidget()->mainView()->setSceneData(osg_root);
	m_window->getViewerWidget()->reset();

	emit sReset();

	m_navigation_control->a_object->trigger(); // goto object mode
	return true;
}

VSimRoot *VSimApp::getRoot() const
{
	return m_root.get();
}

std::string VSimApp::getLastDiretory() const
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

std::string VSimApp::getCurrentDirectory() const
{
	QFileInfo f(m_filename.c_str());
	return f.dir().path().toStdString();
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

QImage VSimApp::generateThumbnail(NarrativeSlide * slide)
{
	QImage img(m_thumbnail_size, QImage::Format_ARGB32);
	QPainter painter(&img);

	// render osg
	m_render_view->setCameraMatrix(slide->getCameraMatrix());
	//m_render_view->render(&painter, QPoint(0, 0), QRect(QPoint(0, 0), m_thumbnail_size), 0);

	// render canvas
	m_render_canvas->setScene(slide->scene());
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

}

//OSGYearModel *VSimApp::modelTable() const
//{
//	return m_model_table_model;
//}

NarrativeControl * VSimApp::narrativeControl() const
{
	return m_narrative_control;
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
