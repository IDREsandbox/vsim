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
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

#include <cassert>

#include <stdexcept>
#include <vector>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>
#include <QSurfaceFormat>

OSGViewerWidget::OSGViewerWidget(QWidget* parent, Qt::WindowFlags f)
	: QOpenGLWidget(parent,	f)
{
	// Create viewer, graphics context, and link them together
	viewer_ = new osgViewer::Viewer;
	//osg::ref_ptr<osg::DisplaySettings> display_settings = new osg::DisplaySettings;
	//display_settings->setNumMultiSamples(8);
	//viewer_->setDisplaySettings(display_settings);

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(osg::DisplaySettings::instance());
	traits->x = x();
	traits->y = y();
	traits->width = width();
	traits->height = height();
	graphicsWindow_ = new osgViewer::GraphicsWindowEmbedded(traits);
	
	float aspectRatio = this->width() / (float)this->height();
	
	// Create an empty scene
	osg::Group* group = new osg::Group();
	viewer_->setSceneData(group);

	// Use default camera
	osg::Camera* camera = viewer_->getCamera();
	camera->setViewport(0, 0, this->width(), this->height());
	camera->setClearColor(osg::Vec4(51/255.f, 51/255.f, 102/255.f, 1.f));
	camera->setProjectionMatrixAsPerspective(55.f, aspectRatio, 1.f, 7500.f);
	camera->setGraphicsContext(graphicsWindow_); // set the context

	// Stats Handler
	osgViewer::StatsHandler *stats_handler = new osgViewer::StatsHandler;
	stats_handler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_T);
	stats_handler->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_Y);
	viewer_->addEventHandler(stats_handler);
	
	// Camera State Handler
	osgGA::StateSetManipulator* ssm = new osgGA::StateSetManipulator(viewer_->getCamera()->getOrCreateStateSet());
	ssm->setKeyEventToggleBackfaceCulling('B');
	ssm->setKeyEventToggleLighting('L');
	ssm->setKeyEventToggleTexturing('X');
	ssm->setKeyEventCyclePolygonMode('M');
	ssm->setLightingEnabled(false);
	viewer_->addEventHandler(ssm);

	// Lighting
	viewer_->setLightingMode(osg::View::SKY_LIGHT);
	
	// Anti aliasing (QOpenGLWidget)
	QSurfaceFormat fmt = this->format();
	fmt.setSamples(4);
	this->setFormat(fmt);

	// Camera Manipulator and Navigation
	//osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
	//manipulator->setAllowThrow(false);
	m_camera_frozen = false;
	m_simple_manipulator = new SimpleCameraManipulator();
	m_first_person_manipulator = new FirstPersonManipulator;
	m_flight_manipulator = new FlightManipulator;
	m_object_manipulator = new ObjectManipulator;
	viewer_->setCameraManipulator(m_object_manipulator);
	m_navigation_mode = NAVIGATION_OBJECT;

	m_collisions_on = false;
	m_gravity_on = false;
	//setNavigationMode(NAVIGATION_OBJECT);
	
	// This ensures that the widget will receive keyboard events. This focus
	// policy is not set by default. The default, Qt::NoFocus, will result in
	// keyboard events that are ignored.
	this->setFocusPolicy(Qt::StrongFocus);
	this->setMinimumSize(100, 100);

	// Key tracking
	this->installEventFilter(&m_key_tracker);

	// Mouse tracking
	this->setMouseTracking(true);

	// frame timer
	m_frame_timer.start();
}

osgViewer::Viewer* OSGViewerWidget::getViewer() const
{
	return viewer_;
}

osg::Matrixd OSGViewerWidget::getCameraMatrix()
{
	return viewer_->getCameraManipulator()->getMatrix();
}

void OSGViewerWidget::setCameraMatrix(osg::Matrixd m)
{
	return viewer_->getCameraManipulator()->setByMatrix(m);
}

void OSGViewerWidget::setNavigationMode(NavigationMode mode)
{
	
	osg::Matrixd old_matrix = getCameraMatrix();

	osgGA::CameraManipulator *new_manipulator;
	switch (mode) {
	case NAVIGATION_SIMPLE:
		qInfo() << "Navigation mode set to Simple";
		new_manipulator = m_simple_manipulator;
		break;
	case NAVIGATION_FIRST_PERSON:
		qInfo() << "Navigation mode set to First Person";
		new_manipulator = m_first_person_manipulator;
		m_first_person_manipulator->stop();
		break;
	case NAVIGATION_FLIGHT:
		qInfo() << "Navigation mode set to Flight";
		new_manipulator = m_flight_manipulator;
		m_flight_manipulator->stop();
		break;
	case NAVIGATION_OBJECT:
	default:
		qInfo() << "Navigation mode set to Object";
		new_manipulator = m_object_manipulator;
		m_object_manipulator->finishAnimation(); // same as stop
		break;
	}

	m_navigation_mode = mode;

	if (m_camera_frozen) {
		return;
	}

	if (mode == NAVIGATION_FIRST_PERSON) {
		takeCursor();
	}
	else {
		releaseCursor();
	}

	viewer_->setCameraManipulator(new_manipulator, false);
	setCameraMatrix(old_matrix);
}

OSGViewerWidget::NavigationMode OSGViewerWidget::getNavigationMode() const
{
	return m_navigation_mode;
}

OSGViewerWidget::NavigationMode OSGViewerWidget::getActualNavigationMode() const
{
	return m_camera_frozen ? NAVIGATION_SIMPLE : m_navigation_mode;
}

void OSGViewerWidget::setCameraFrozen(bool freeze)
{
	m_camera_frozen = freeze;
	if (freeze) {
		qInfo() << "Camera freeze";
		osg::Matrixd old_matrix = getCameraMatrix();
		viewer_->setCameraManipulator(m_simple_manipulator);
		setCameraMatrix(old_matrix);
		releaseCursor();

		// stop all manipulator momentum
		m_first_person_manipulator->stop();
		m_flight_manipulator->stop();
		m_object_manipulator->finishAnimation();
	}
	else {
		qInfo() << "Camera unfreeze";
		setNavigationMode(m_navigation_mode);
		setFocus();
	}
}

bool OSGViewerWidget::getCameraFrozen() const
{
	return m_camera_frozen;
}



void OSGViewerWidget::reset()
{
	// compute home position and all that
	osgGA::CameraManipulator *cm = viewer_->getCameraManipulator();
	cm->setAutoComputeHomePosition(true);
	cm->setNode(viewer_->getSceneData());
	cm->computeHomePosition();
	viewer_->home();
	qDebug() << "home position";
}

void OSGViewerWidget::paintEvent(QPaintEvent *e)
{
	// a frame
	qint64 dt = m_frame_timer.nsecsElapsed();
	m_frame_timer.restart();
	double dt_sec = (double)dt / 1.0e9;

	NavigationMode mode = getActualNavigationMode();
	if (mode == NAVIGATION_FIRST_PERSON) {
		m_first_person_manipulator->update(dt_sec, &m_key_tracker, viewer_->getSceneData());
	}
	else if (mode == NAVIGATION_FLIGHT) {
		QPoint mouse = mapFromGlobal(cursor().pos());
		int dx = mouse.x() - width() / 2;
		int dy = mouse.y() - height() / 2;
		double nx = 2 * dx / (double)height();
		double ny = 2 * dy / (double)height();
		m_flight_manipulator->setMousePosition(nx, ny);

		m_flight_manipulator->update(dt_sec, &m_key_tracker, viewer_->getSceneData());
	}


	// let qt do setup stuff and call paintGL
	QOpenGLWidget::paintEvent(e);
}

void OSGViewerWidget::paintGL()
{
	viewer_->frame();
}

void OSGViewerWidget::resizeGL(int width, int height)
{
	this->getEventQueue()->windowResize(this->x(), this->y(), width, height);
	graphicsWindow_->resized(this->x(), this->y(), width, height);

	this->onResize(width, height);
}

void OSGViewerWidget::keyPressEvent(QKeyEvent* event)
{
	NavigationMode mode = getActualNavigationMode();
	if (mode == NAVIGATION_FLIGHT) {
		if (event->key() == Qt::Key_Alt) {
			m_flight_manipulator->stop();
			takeCursor();
		}
	}
	if (event->key() == Qt::Key_G) {
		qInfo() << "Gravity on";
		// Toggle gravity
		m_gravity_on = !m_gravity_on;
		m_flight_manipulator->enableGravity(m_gravity_on);
		m_first_person_manipulator->enableGravity(m_gravity_on);
	}
	if (event->key() == Qt::Key_C) {
		// Toggle collisions
		m_collisions_on = !m_collisions_on;
		qInfo() << "Collision" << m_collisions_on;
		m_flight_manipulator->enableCollisions(m_collisions_on);
		m_first_person_manipulator->enableCollisions(m_collisions_on);
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
	NavigationMode mode = getActualNavigationMode();
	if (mode == NAVIGATION_FLIGHT) {
		if (event->key() == Qt::Key_Alt) {
			m_flight_manipulator->stop();
			releaseCursor();
		}
	}

	QString keyString = event->text();
	QByteArray keyData = keyString.toLocal8Bit();
	this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*keyData.data()));
}

int i = 0;
void OSGViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
	i++;
	NavigationMode mode = getActualNavigationMode();
	int dx = event->x() - width() / 2;
	int dy = event->y() - height() / 2;
	if (mode == NAVIGATION_FIRST_PERSON) {
		// if the mouse is centered then don't do anything
		if (dx != 0 || dy != 0) {
			m_first_person_manipulator->mouseMove(dx, dy);
			centerCursor();
		}
	}
	if (mode == NAVIGATION_FLIGHT) {
		if (m_key_tracker.mouseButton(Qt::MiddleButton) || m_key_tracker.keyPressed(Qt::Key_Alt)) {
			if (dx != 0 || dy != 0) {
				m_flight_manipulator->strafe(dx, dy);
				centerCursor();
			}
		}
	}
	// We'd have to install a global event filter for all mouse move events to go
	//  through, it's easier just to poll on update.
	//else if (mode == NAVIGATION_FLIGHT) {
	//	...
	//	m_flight_manipulator->setMousePosition(nx, ny);
	//}

	event->accept();
	this->getEventQueue()->mouseMotion(static_cast<float>(event->x()),
		static_cast<float>(event->y()));
}

void OSGViewerWidget::mousePressEvent(QMouseEvent* event)
{
	NavigationMode mode = getActualNavigationMode();
	if (mode == NAVIGATION_FLIGHT) {
		if (event->button() == Qt::MiddleButton) {
			m_flight_manipulator->stop();
			takeCursor();
		}
	}

	// Normal processing

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
	NavigationMode mode = getActualNavigationMode();
	if (mode == NAVIGATION_FLIGHT) {
		if (event->button() == Qt::MiddleButton) {
			m_flight_manipulator->stop();
			releaseCursor();
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

	if (getActualNavigationMode() == NAVIGATION_FIRST_PERSON) {
		//qDebug() << "mouse wheel delta" << delta;
		m_first_person_manipulator->accelerate((delta > 0) ? 1 : -1);
	}

	osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP
		: osgGA::GUIEventAdapter::SCROLL_DOWN;

	this->getEventQueue()->mouseScroll(motion);
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
	viewer_->getViews(views);

	for (std::size_t i = 0; i < views.size(); i++)
	{
		osgViewer::View* view = views.at(i);
		view->home();
	}
}

void OSGViewerWidget::onResize(int width, int height)
{
	std::vector<osg::Camera*> cameras;
	viewer_->getCameras(cameras);

	if (cameras.size() > 0) {
		cameras[0]->setViewport(0, 0, width, height);
	}
}

void OSGViewerWidget::centerCursor()
{
	QCursor new_cursor = cursor();
	new_cursor.setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
	setCursor(new_cursor);
}

void OSGViewerWidget::takeCursor()
{
	grabMouse(Qt::BlankCursor);
	centerCursor();	
}

void OSGViewerWidget::releaseCursor()
{
	releaseMouse();
}

osgGA::EventQueue* OSGViewerWidget::getEventQueue() const
{
	osgGA::EventQueue* eventQueue = graphicsWindow_->getEventQueue();

	if (eventQueue)
		return eventQueue;
	else
		throw std::runtime_error("Unable to obtain valid event queue");
}