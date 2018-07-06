#include "OSGViewerWidget.h"

#include <osg/Camera>

#include <osg/DisplaySettings>

#include <osgDB/WriteFile>
#include <osgDB/ReadFile>

#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include <osgViewer/View>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

#include <cassert>

#include <stdexcept>
#include <vector>

#include <QOpenGLFunctions_2_0>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>
#include <QSurfaceFormat>
#include <QTimer>
#include <QAction>

#include "SimpleCameraManipulator.h"
#include "FirstPersonManipulator.h"
#include "FlightManipulator.h"
#include "ObjectManipulator.h"
#include "KeyTracker.h"

OSGViewerWidget::OSGViewerWidget(QWidget* parent, Qt::WindowFlags f)
	: QOpenGLWidget(parent,	f),
	m_rendering_enabled(true)
{
	// Qt Stuff
	// Anti aliasing (QOpenGLWidget)
	QSurfaceFormat fmt;
	fmt.setSamples(0);
	setFormat(fmt);

	// Create viewer, graphics context, and link them together
	m_main_view = new osgViewer::View();

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(osg::DisplaySettings::instance());
	traits->x = x();
	traits->y = y();
	traits->width = width();
	traits->height = height();
	graphicsWindow_ = new osgViewer::GraphicsWindowEmbedded(traits);

	float aspectRatio = this->width() / (float)this->height();

	// Create an empty scene
	osg::Group* group = new osg::Group();
	m_main_view->setSceneData(group);

	// Use default camera
	osg::Camera* camera = m_main_view->getCamera();
	camera->setViewport(0, 0, this->width(), this->height());
	camera->setClearColor(osg::Vec4(51/255.f, 51/255.f, 102/255.f, 1.f));
	camera->setProjectionMatrixAsPerspective(55.f, aspectRatio, 1.f, 7500.f);
	camera->setGraphicsContext(graphicsWindow_); // set the context, connects the viewer and graphics context
	//camera->setCullingMode(osg::Camera::NO_CULLING);

	// Stats Handler
	m_stats = new osgViewer::StatsHandler;
	m_stats->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F30); // biggest hack
	m_stats->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_F29);
	m_main_view->addEventHandler(m_stats);

	// Camera State Handler
	m_ssm = new osgGA::StateSetManipulator(m_main_view->getCamera()->getOrCreateStateSet());
	//ssm->setKeyEventToggleBackfaceCulling('B');
	//ssm->setKeyEventToggleLighting('L');
	//ssm->setKeyEventToggleTexturing('X');
	//ssm->setKeyEventCyclePolygonMode('M');
	m_ssm->setLightingEnabled(true);
	m_ssm->setBackfaceEnabled(true);
	m_ssm->setTextureEnabled(true);
	m_ssm->setPolygonMode(osg::PolygonMode::FILL);
	//m_main_view->addEventHandler(ssm);
	//auto ss = m_main_view->getCamera()->getStateSet();

	// Lighting
	m_main_view->setLightingMode(osg::View::SKY_LIGHT);

	// Camera Manipulator and Navigation
	//osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
	//manipulator->setAllowThrow(false);
	m_camera_frozen = false;
	m_simple_manipulator = new SimpleCameraManipulator();
	m_first_person_manipulator = new FirstPersonManipulator;
	m_flight_manipulator = new FlightManipulator;
	m_object_manipulator = new ObjectManipulator;
	m_main_view->setCameraManipulator(m_object_manipulator);
	m_manipulator = MANIPULATOR_OBJECT;
	m_navigation_mode = Navigation::OBJECT;

	m_collisions_on = false;
	m_gravity_on = false;

	// speed
	a_speed_up = new QAction(this);
	a_speed_up->setText("Speed up");
	a_speed_up->setShortcut(QKeySequence(Qt::Key_Equal));
	addAction(a_speed_up);
	connect(a_speed_up, &QAction::triggered, this, [this]() {
		adjustSpeed(1);
		emit sSpeedActivelyChanged(m_speed_tick,
			m_flight_manipulator->getSpeedMultiplier());
	});

	a_slow_down = new QAction(this);
	a_slow_down->setText("Slow down");
	a_slow_down->setShortcut(QKeySequence(Qt::Key_Minus));
	addAction(a_slow_down);
	connect(a_slow_down, &QAction::triggered, this, [this]() {
		adjustSpeed(-1);
		emit sSpeedActivelyChanged(m_speed_tick,
			m_flight_manipulator->getSpeedMultiplier());
	});

	m_speed_tick = 0;
	m_first_person_manipulator->setSpeedTick(m_speed_tick);
	m_flight_manipulator->setSpeedTick(m_speed_tick);

	// This ensures that the widget will receive keyboard events. This focus
	// policy is not set by default. The default, Qt::NoFocus, will result in
	// keyboard events that are ignored.
	this->setFocusPolicy(Qt::StrongFocus);
	//this->setMinimumSize(100, 100);

	// Key tracking
	m_key_tracker = new KeyTracker(this);
	this->installEventFilter(m_key_tracker);
	this->installEventFilter(this);

	// Mouse tracking
	this->setMouseTracking(true);

	// frame timer
	m_frame_timer.start();

	// always render timer
	QTimer *t = new QTimer(this);
	t->setInterval(0);
	t->start();
	connect(t, &QTimer::timeout, this, [this]() {update();});
}

osgViewer::ViewerBase* OSGViewerWidget::getViewer() const
{
	return m_viewer;
}

osgViewer::View * OSGViewerWidget::mainView() const
{
	return m_main_view;
}

osgGA::StateSetManipulator * OSGViewerWidget::getStateSetManipulator()
{
	return m_ssm;
}

void OSGViewerWidget::cycleStats()
{
	this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_F30);
	//m_stats->handle();
}

osg::Matrixd OSGViewerWidget::getCameraMatrix()
{
	return m_main_view->getCameraManipulator()->getMatrix();
}

void OSGViewerWidget::setCameraMatrix(osg::Matrixd m)
{
	return m_main_view->getCameraManipulator()->setByMatrix(m);
}

void OSGViewerWidget::setNavigationMode(Navigation::Mode mode)
{
	m_camera_frozen = false;
	m_navigation_mode = mode;

	figureOutNavigation();

	emit sCameraFrozen(false);
	emit sNavigationModeChanged(mode);
}

Navigation::Mode OSGViewerWidget::getNavigationMode() const
{
	return m_navigation_mode;
}

Manipulator OSGViewerWidget::getManipulator() const
{
	return m_manipulator;
}

Navigation::Mode OSGViewerWidget::manipulatorToNavigation(Manipulator man)
{
	switch (man) {
	case MANIPULATOR_FIRST_PERSON:
		return Navigation::FIRST_PERSON;
	case MANIPULATOR_FLIGHT:
		return Navigation::FLIGHT;
	case MANIPULATOR_OBJECT:
		return Navigation::OBJECT;
	}
	return Navigation::FIRST_PERSON;
}
Manipulator OSGViewerWidget::navigationToManipulator(Navigation::Mode mode)
{
	switch (mode) {
	case Navigation::FIRST_PERSON:
		return MANIPULATOR_FIRST_PERSON;
	case Navigation::FLIGHT:
		return MANIPULATOR_FLIGHT;
	case Navigation::OBJECT:
		return MANIPULATOR_OBJECT;
	}
	return MANIPULATOR_SIMPLE;
}

void OSGViewerWidget::setManipulator(Manipulator manipulator)
{
	osg::Matrixd old_matrix = getCameraMatrix();

	osgGA::CameraManipulator *new_manipulator;
	m_manipulator = manipulator;

	switch (manipulator) {
	case MANIPULATOR_SIMPLE:
		new_manipulator = m_simple_manipulator;
		break;
	case MANIPULATOR_FIRST_PERSON:
		new_manipulator = m_first_person_manipulator;
		m_first_person_manipulator->stop();
		break;
	case MANIPULATOR_FLIGHT:
		new_manipulator = m_flight_manipulator;
		m_flight_manipulator->stop();
		flightStopStrafe();
		break;
	case MANIPULATOR_OBJECT:
	default:
		new_manipulator = m_object_manipulator;
		m_object_manipulator->finishAnimation(); // same as stop
		break;
	}

	if (manipulator == MANIPULATOR_FIRST_PERSON) {
		takeCursor();
	}
	else {
		releaseCursor();
	}

	m_main_view->setCameraManipulator(new_manipulator, false);
	setCameraMatrix(old_matrix);
}

void OSGViewerWidget::adjustSpeed(int tick)
{
	int speed_limit = 40;
	m_speed_tick += tick;
	m_speed_tick = std::clamp(m_speed_tick, -speed_limit, speed_limit);
	m_first_person_manipulator->setSpeedTick(m_speed_tick);
	m_flight_manipulator->setSpeedTick(m_speed_tick);
}

void OSGViewerWidget::setCameraFrozen(bool freeze)
{
	m_camera_frozen = freeze;
	figureOutNavigation();
	emit sCameraFrozen(freeze);
}

bool OSGViewerWidget::getCameraFrozen() const
{
	return m_camera_frozen;
}

void OSGViewerWidget::figureOutNavigation()
{
	if (!hasFocus()) {
		setManipulator(MANIPULATOR_SIMPLE);
		return;
	}
	if (m_camera_frozen) {
		setManipulator(MANIPULATOR_SIMPLE);
		return;
	}
	setManipulator(navigationToManipulator(m_navigation_mode));
}

void OSGViewerWidget::stopNavigation()
{
	// stop all manipulator momentum
	m_first_person_manipulator->stop();
	m_flight_manipulator->stop();
	m_object_manipulator->finishAnimation();
}

void OSGViewerWidget::flightStartStrafe()
{
	m_flight_manipulator->stop();
	centerCursor();
}

void OSGViewerWidget::flightStopStrafe()
{
	m_flight_manipulator->stop();
	centerCursor();
}

void OSGViewerWidget::enableGravity(bool enable)
{
	qInfo() << "Gravity" << enable;
	m_gravity_on = enable;
	m_flight_manipulator->enableGravity(enable);
	m_first_person_manipulator->enableGravity(enable);
}

void OSGViewerWidget::enableCollisions(bool enable)
{
	qInfo() << "Collision" << enable;
	m_collisions_on = enable;
	m_flight_manipulator->enableCollisions(enable);
	m_first_person_manipulator->enableCollisions(enable);
}

void OSGViewerWidget::enableRendering(bool enable)
{
	m_rendering_enabled = enable;
	if (enable) update();
}

void OSGViewerWidget::setViewer(osgViewer::CompositeViewer * viewer)
{
	m_viewer = viewer;
	m_viewer->addView(m_main_view);
}

void OSGViewerWidget::reset()
{
	// pretty much copied from osgGA::CameraManipulator::computeHomePosition
	qInfo() << "Home";
	osg::BoundingSphere bound = m_main_view->getSceneData()->getBound();

	double left, right, bottom, top, zNear, zFar, dist;
	m_main_view->getCamera()->getProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar);
	double vertical2 = fabs(right - left) / zNear / 2.;
	double horizontal2 = fabs(top - bottom) / zNear / 2.;
	double dim = horizontal2 < vertical2 ? horizontal2 : vertical2;
	double viewAngle = atan2(dim, 1.);
	dist = bound.radius() / sin(viewAngle);

	// for some reason osg::lookAt gives the inverse...
	osg::Matrixd mat = osg::Matrix::lookAt(bound.center() + osg::Vec3d(0.0, -dist, 0.0f), bound.center(), osg::Vec3(0, 0, 1));
	setCameraMatrix(osg::Matrix::inverse(mat));
}

bool OSGViewerWidget::eventFilter(QObject * obj, QEvent * e)
{
	// this is in the key tracker
	//if (e->type() == QEvent::ShortcutOverride) {
	//	if (getActualNavigationMode() == MANIPULATOR_FIRST_PERSON) {
	//		qDebug() << "shortcut override wasd";
	//		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
	//		int key = keyEvent->key();
	//		if (key == 'W' || key == 'A' || key == 'S' || key == 'D') {
	//			e->accept();
	//			return true;
	//		}
	//	}
	//}
	return false;
}

QImage OSGViewerWidget::renderView(QSize size, const osg::Matrixd & matrix)
{
	// copy main view
	osg::ref_ptr<osgViewer::View> view(m_main_view);
	m_main_view->setCameraManipulator(nullptr);

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(osg::DisplaySettings::instance());
	traits->x = 0;
	traits->y = 0;
	traits->width = 300;
	traits->height = 300;
	graphicsWindow_ = new osgViewer::GraphicsWindowEmbedded(traits);

	osg::Camera* camera = m_main_view->getCamera();
	camera->setGraphicsContext(graphicsWindow_);

	//m_viewer->addView(view);

	return QImage();
}

float OSGViewerWidget::getFrameTime() const
{
	return m_frame_time;
}

float OSGViewerWidget::getTimeBetween() const
{
	return m_time_between;
}

float OSGViewerWidget::getFullFrameTime() const
{
	return m_full_frame_time;
}

void OSGViewerWidget::paintEvent(QPaintEvent *e)
{
	// let qt do setup stuff and call paintGL
	QOpenGLWidget::paintEvent(e);
}

void OSGViewerWidget::initializeGL()
{

}

void OSGViewerWidget::paintGL()
{
	m_fbo->bind();

	QElapsedTimer t; t.start();
	if (!m_viewer) return;
	//qDebug() << "paint gl" << this << m_viewer->getNumViews();
	qint64 dt = m_frame_timer.nsecsElapsed();
	m_frame_timer.restart();
	double dt_sec = (double)dt / 1.0e9;

	m_time_between = m_between_timer.nsecsElapsed() / 1000000.0;

	//emit frame(dt_sec);

	if (m_manipulator == MANIPULATOR_FIRST_PERSON) {
		m_first_person_manipulator->update(dt_sec, m_key_tracker, m_main_view->getSceneData());
	}
	else if (m_manipulator == MANIPULATOR_FLIGHT) {
		// shouldn't be in mousemove because entering flight mode doesn't necessarily
		// mean moving the mouse
		QPoint mouse = mapFromGlobal(cursor().pos());
		int dx = mouse.x() - width() / 2;
		int dy = mouse.y() - height() / 2;
		double nx = 2 * dx / (double)height();
		double ny = 2 * dy / (double)height();
		m_flight_manipulator->setMousePosition(nx, ny);

		m_flight_manipulator->update(dt_sec, m_key_tracker, m_main_view->getSceneData());

	}

	// hacky way that lets us have multiple OSGViewerWidgets sharing one CompositeViewer
	//m_viewer->addView(m_main_view);
	//frame()
	//m_viewer->removeView(m_main_view);
	// this is really slow for some reason
	if (m_rendering_enabled) {
		QElapsedTimer ft; ft.start();
		m_viewer->frame(dt);
		m_frame_time = ft.nsecsElapsed() / 1000000.0;
	}

	m_full_frame_time = t.nsecsElapsed() / 1000000.0;
	m_between_timer.start();

	QOpenGLFramebufferObject::blitFramebuffer(nullptr, m_fbo.get(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void OSGViewerWidget::resizeGL(int width, int height)
{
	this->getEventQueue()->windowResize(this->x(), this->y(), width, height);
	graphicsWindow_->resized(this->x(), this->y(), width, height);

	std::vector<osg::Camera*> cameras;
	if (!m_viewer) return;
	m_viewer->getCameras(cameras);

	m_main_view->getCamera()->setViewport(0, 0, width, height);

	recreateFramebuffer();
}

void OSGViewerWidget::keyPressEvent(QKeyEvent* event)
{
	if (m_manipulator == MANIPULATOR_FLIGHT) {
		if (event->key() == Qt::Key_Alt) {
			flightStartStrafe();
		}
	}

	// This guy's code is a little wonky, notice that toLocal8Bit() returns a temporary array, but data
	// is a pointer to memory. This led to a bug where calling qDebug() after this line would override
	// keyData (since it's just an arbitrary place in memory)
	// const char* keyData = keyString.toLocal8Bit().data();
	// Safer code
	QString keyString = event->text();
	QByteArray keyData = keyString.toLocal8Bit();
	this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol(*keyData.data()));
}

void OSGViewerWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (m_manipulator == MANIPULATOR_FLIGHT) {
		if (event->key() == Qt::Key_Alt) {
			flightStopStrafe();
		}
	}

	QString keyString = event->text();
	QByteArray keyData = keyString.toLocal8Bit();
	this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*keyData.data()));
}

void OSGViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
	int dx = event->x() - width() / 2;
	int dy = event->y() - height() / 2;
	if (m_manipulator == MANIPULATOR_FIRST_PERSON) {
		// if the mouse is centered then don't do anything
		if (dx != 0 || dy != 0) {
			m_first_person_manipulator->mouseMove(dx, dy);
			centerCursor();
		}
	}
	if (m_manipulator == MANIPULATOR_FLIGHT) {
		m_flight_manipulator->setMousePosition(dx, dy);
		// use absolute mouse position instead
		//if (m_key_tracker.mouseButton(Qt::MiddleButton) || m_key_tracker.keyPressed(Qt::Key_Alt)) {
		//	if (dx != 0 || dy != 0) {
		//		m_flight_manipulator->strafe(dx, dy);
		//		centerCursor();
		//	}
		//}
	}
	// We'd have to install a global event filter for all mouse move events to go
	//  through, it's easier just to poll on update.

	event->accept();
	this->getEventQueue()->mouseMotion(static_cast<float>(event->x()),
		static_cast<float>(event->y()));
}

void OSGViewerWidget::mousePressEvent(QMouseEvent* event)
{
	if (m_manipulator == MANIPULATOR_FLIGHT) {
		if (event->button() == Qt::MiddleButton) {
			flightStartStrafe();
		}
	}

	// 1 = left mouse button
	// 2 = middle mouse button
	// 3 = right mouse button
	unsigned int button = 0;
	switch (event->button())
	{
	case Qt::LeftButton:
		button = 1;
		break;

	case Qt::MiddleButton:
		button = 2;
		break;

	case Qt::RightButton:
		button = 3;
		break;

	default:
		break;
	}
	this->getEventQueue()->mouseButtonPress(static_cast<float>(event->x()),
		static_cast<float>(event->y()),
		button);

}

void OSGViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_manipulator == MANIPULATOR_FLIGHT) {
		if (event->button() == Qt::MiddleButton) {
			flightStopStrafe();
		}
	}
	// 1 = left mouse button
	// 2 = middle mouse button
	// 3 = right mouse button

	unsigned int button = 0;

	switch (event->button())
	{
	case Qt::LeftButton:
		button = 1;
		break;

	case Qt::MiddleButton:
		button = 2;
		break;

	case Qt::RightButton:
		button = 3;
		break;

	default:
		break;
	}

	this->getEventQueue()->mouseButtonRelease(static_cast<float>(event->x()),
		static_cast<float>(event->y()),
		button);
}

void OSGViewerWidget::wheelEvent(QWheelEvent* event) {

	event->accept();
	int delta = event->delta();

	if (m_manipulator == MANIPULATOR_FIRST_PERSON
		|| m_manipulator == MANIPULATOR_FLIGHT)
	{
		if (delta > 0) {
			a_speed_up->trigger();
		}
		else {
			a_slow_down->trigger();
		}
	}

	// osg adapter
	osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP
		: osgGA::GUIEventAdapter::SCROLL_DOWN;
	this->getEventQueue()->mouseScroll(motion);
}

void OSGViewerWidget::focusOutEvent(QFocusEvent * event)
{
	releaseMouse();
	figureOutNavigation();
}

void OSGViewerWidget::focusInEvent(QFocusEvent * event)
{
	figureOutNavigation();
}

bool OSGViewerWidget::event(QEvent* event)
{
	bool handled = QOpenGLWidget::event(event);

	// This ensures that the OSG widget is always going to be repainted after the
	// user performed some interaction. Doing this in the event handler ensures
	// that we don't forget about some event and prevents duplicate code.
	switch (event->type())
	{
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove:
	case QEvent::Wheel:
		this->update();
		break;

	default:
		break;
	}

	return handled;
}

void OSGViewerWidget::onHome()
{
	osgViewer::ViewerBase::Views views;
	m_viewer->getViews(views);

	for (std::size_t i = 0; i < views.size(); i++)
	{
		osgViewer::View* view = views.at(i);
		view->home();
	}
}

void OSGViewerWidget::centerCursor()
{
	QCursor new_cursor = cursor();
	new_cursor.setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
	setCursor(new_cursor);
}

void OSGViewerWidget::recreateFramebuffer()
{
	makeCurrent();

	QOpenGLFramebufferObjectFormat fmt;
	fmt.setSamples(8);
	fmt.setAttachment(QOpenGLFramebufferObject::Depth);

	m_fbo = std::make_unique<QOpenGLFramebufferObject>(size(), fmt);
}

void OSGViewerWidget::takeCursor()
{
	//grabMouse(); // this is really buggy... just don't use it
	setCursor(Qt::BlankCursor);
	centerCursor();
	m_key_tracker->stealKeys({Qt::Key_W, Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_Shift, Qt::Key_Control});
}

void OSGViewerWidget::releaseCursor()
{
	setCursor(Qt::ArrowCursor);
	m_key_tracker->stealKeys({});
}

osgGA::EventQueue* OSGViewerWidget::getEventQueue() const
{
	osgGA::EventQueue* eventQueue = graphicsWindow_->getEventQueue();

	if (eventQueue)
		return eventQueue;
	else
		throw std::runtime_error("Unable to obtain valid event queue");
}