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

#include <cassert>

#include <stdexcept>
#include <vector>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>

OSGViewerWidget::OSGViewerWidget(QWidget* parent, Qt::WindowFlags f)
	: QOpenGLWidget(parent,	f)
	, graphicsWindow_(new osgViewer::GraphicsWindowEmbedded(this->x(),
		this->y(),
		this->width(),
		this->height()))
	, viewer_(new osgViewer::Viewer)
{

	float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());

	osg::Camera* camera = new osg::Camera;
	camera->setViewport(0, 0, this->width(), this->height());
	camera->setClearColor(osg::Vec4(51/255.f, 51/255.f, 102/255.f, 1.f));
	camera->setProjectionMatrixAsPerspective(55.f, aspectRatio, 1.f, 7500.f);
	camera->setGraphicsContext(graphicsWindow_);
	camera->getOrCreateStateSet()->setGlobalDefaults(); // depth buffers and things

	//osgViewer::View* view = new osgViewer::View;
	viewer_->setCamera(camera);
	
	osg::Group* group = new osg::Group();
	viewer_->setSceneData(group);
	
	osgViewer::StatsHandler *stats_handler = new osgViewer::StatsHandler;
	stats_handler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_T);
	stats_handler->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_Y);
	viewer_->addEventHandler(stats_handler);
	
	// Camera Manipulator and Navigation
	//osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
	//manipulator->setAllowThrow(false);
	//viewer_->setCameraManipulator(manipulator);
	m_camera_frozen = false;
	m_simple_manipulator = new SimpleCameraManipulator();
	m_first_person_manipulator = new FirstPersonManipulator;
	m_flight_manipulator = new FlightManipulator;
	m_object_manipulator = new ObjectManipulator;
	viewer_->setCameraManipulator(m_object_manipulator);
	//setNavigationMode(NAVIGATION_OBJECT);

	// lighting
	viewer_->setLightingMode(osg::View::SKY_LIGHT);

	// for composite viewer
	// viewer_->addView(view);
	// viewer_->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
	// viewer_->realize();

	// This ensures that the widget will receive keyboard events. This focus
	// policy is not set by default. The default, Qt::NoFocus, will result in
	// keyboard events that are ignored.
	this->setFocusPolicy(Qt::StrongFocus);
	this->setMinimumSize(100, 100);

	// Key tracking
	this->installEventFilter(&m_key_tracker);

	// Ensures that the widget receives mouse move events even though no
	// mouse button has been pressed. We require this in order to let the
	// graphics window switch viewports properly.
	this->setMouseTracking(true);

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

void OSGViewerWidget::paintEvent(QPaintEvent* /* paintEvent */)
{
	// a frame
	qint64 dt = m_frame_timer.nsecsElapsed();
	m_frame_timer.restart();
	double dt_sec = (double)dt / 1.0e9;

	NavigationMode mode = getActualNavigationMode();
	if (mode == NAVIGATION_FIRST_PERSON) {
		m_first_person_manipulator->update(dt_sec, &m_key_tracker);
	}
	else if (mode == NAVIGATION_FLIGHT) {
		QPoint mouse = mapFromGlobal(cursor().pos());
		int dx = mouse.x() - width() / 2;
		int dy = mouse.y() - height() / 2;
		double nx = 2 * dx / (double)height();
		double ny = 2 * dy / (double)height();
		m_flight_manipulator->setMousePosition(nx, ny);

		m_flight_manipulator->update(dt_sec, &m_key_tracker);
	}

	this->makeCurrent();

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	this->paintGL();

	painter.end();

	this->doneCurrent();
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