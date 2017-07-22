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
	, selectionActive_(false)
	, selectionFinished_(true)
{

	float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());

	osg::Camera* camera = new osg::Camera;
	camera->setViewport(0, 0, this->width(), this->height());
	camera->setClearColor(osg::Vec4(51/255.f, 51/255.f, 102/255.f, 1.f));
	camera->setProjectionMatrixAsPerspective(75.f, aspectRatio, 1.f, 7500.f);
	camera->setGraphicsContext(graphicsWindow_);
	camera->getOrCreateStateSet()->setGlobalDefaults(); // depth buffers and things

	//osgViewer::View* view = new osgViewer::View;
	viewer_->setCamera(camera);
	
	osg::Group* group = new osg::Group();
	viewer_->setSceneData(group);
	
	osgViewer::StatsHandler *stats_handler = new osgViewer::StatsHandler;
	stats_handler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_T);
	viewer_->addEventHandler(stats_handler);
	
	// Camera Manipulator and Navigation
	//osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
	//manipulator->setAllowThrow(false);
	//viewer_->setCameraManipulator(manipulator);
	m_camera_frozen = false;
	m_simple_manipulator = new SimpleCameraManipulator();
	m_first_person_manipulator = new FirstPersonManipulator;
	m_flight_manipulator = new FirstPersonManipulator;
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

		m_first_person_manipulator->stop();

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

	if (getActualNavigationMode() == NAVIGATION_FIRST_PERSON) {
		m_first_person_manipulator->update(dt_sec, &m_key_tracker);
	}

	this->makeCurrent();

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	this->paintGL();

// same as before?
//#ifdef WITH_SELECTION_PROCESSING
//	if (selectionActive_ && !selectionFinished_)
//	{
//		painter.setPen(Qt::black);
//		painter.setBrush(Qt::transparent);
//		painter.drawRect(makeRectangle(selectionStart_, selectionEnd_));
//	}
//#endif

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
	QString keyString = event->text();
	const char* keyData = keyString.toLocal8Bit().data();
	qDebug() << "qt key press" << keyData;

	if (event->key() == Qt::Key_S)
	{
#ifdef WITH_SELECTION_PROCESSING
		selectionActive_ = !selectionActive_;
#endif

		// Further processing is required for the statistics handler here, so we do
		// not return right away.
	}
	else if (event->key() == Qt::Key_D)
	{
		//osgDB::writeNodeFile(*viewer_->getSceneData(),
		//	"/tmp/sceneGraph.osg");

		//return;
	}
	else if (event->key() == Qt::Key_H)
	{
		//this->onHome();
		//return;
	}
	else if (event->key() == Qt::Key_Shift) {
		qDebug() << "qt shift key";
	}
	else if (event->key() == Qt::Key_Alt) {
		qDebug() << "at lkey";
	}
	else if (event->key() == Qt::CTRL) {
		qDebug() << "ctrl";
	}
	this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol(*keyData));
}

void OSGViewerWidget::keyReleaseEvent(QKeyEvent* event)
{
	QString keyString = event->text();
	const char* keyData = keyString.toLocal8Bit().data();

	this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*keyData));
}

bool second = false;
void OSGViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (getActualNavigationMode() == NAVIGATION_FIRST_PERSON) {
		int dx = event->x() - width() / 2;
		int dy = event->y() - height() / 2;
		if (dx == 0 && dy == 0) {
			return;
		}
		m_first_person_manipulator->mouseMove(dx, dy);
		centerCursor();
	}

	event->accept();
	this->getEventQueue()->mouseMotion(static_cast<float>(event->x()),
		static_cast<float>(event->y()));
}

void OSGViewerWidget::mousePressEvent(QMouseEvent* event)
{
	// Selection processing
	if (selectionActive_ && event->button() == Qt::LeftButton)
	{
		selectionStart_ = event->pos();
		selectionEnd_ = selectionStart_; // Deletes the old selection
		selectionFinished_ = false;           // As long as this is set, the rectangle will be drawn
	}

	// Normal processing
	else
	{
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
}

void OSGViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
	// Selection processing: Store end position and obtain selected objects
	// through polytope intersection.
	if (selectionActive_ && event->button() == Qt::LeftButton)
	{
		selectionEnd_ = event->pos();
		selectionFinished_ = true; // Will force the painter to stop drawing the
								   // selection rectangle

		this->processSelection();
	}

	// Normal processing
	else
	{
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
}

void OSGViewerWidget::wheelEvent(QWheelEvent* event)
{
	// Ignore wheel events as long as the selection is active.
	if (selectionActive_)
		return;

	event->accept();
	int delta = event->delta();

	if (getActualNavigationMode() == NAVIGATION_FIRST_PERSON) {
		qDebug() << "mouse wheel delta" << delta;
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

void OSGViewerWidget::processSelection()
{
#ifdef WITH_SELECTION_PROCESSING
	QRect selectionRectangle = makeRectangle(selectionStart_, selectionEnd_);
	int widgetHeight = this->height();

	double xMin = selectionRectangle.left();
	double xMax = selectionRectangle.right();
	double yMin = widgetHeight - selectionRectangle.bottom();
	double yMax = widgetHeight - selectionRectangle.top();

	osgUtil::PolytopeIntersector* polytopeIntersector
		= new osgUtil::PolytopeIntersector(osgUtil::PolytopeIntersector::WINDOW,
			xMin, yMin,
			xMax, yMax);

	// This limits the amount of intersections that are reported by the
	// polytope intersector. Using this setting, a single drawable will
	// appear at most once while calculating intersections. This is the
	// preferred and expected behaviour.
	polytopeIntersector->setIntersectionLimit(osgUtil::Intersector::LIMIT_ONE_PER_DRAWABLE);

	osgUtil::IntersectionVisitor iv(polytopeIntersector);

	for (unsigned int viewIndex = 0; viewIndex < viewer_->getNumViews(); viewIndex++)
	{
		osgViewer::View* view = viewer_->getView(viewIndex);

		if (!view)
			throw std::runtime_error("Unable to obtain valid view for selection processing");

		osg::Camera* camera = view->getCamera();

		if (!camera)
			throw std::runtime_error("Unable to obtain valid camera for selection processing");

		camera->accept(iv);

		if (!polytopeIntersector->containsIntersections())
			continue;

		auto intersections = polytopeIntersector->getIntersections();

		for (auto&& intersection : intersections)
			qDebug() << "Selected a drawable:" << QString::fromStdString(intersection.drawable->getName());
	}
#endif
}
