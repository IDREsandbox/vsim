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
	: QOpenGLWidget(parent,	
		f)
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
	camera->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);
	camera->setGraphicsContext(graphicsWindow_);
	camera->getOrCreateStateSet()->setGlobalDefaults(); // depth buffers and things

	//osgViewer::View* view = new osgViewer::View;
	viewer_->setCamera(camera);
	osg::Node* scene = osgDB::readNodeFile("T:\\Projects\\_UCLA\\vsim\\vsim-dependencies\\OpenSceneGraph-Data-3.4.0\\cow.osg");
	viewer_->setSceneData(scene);
	viewer_->addEventHandler(new osgViewer::StatsHandler);

// I have no idea what this is for. If we need it for additional events copy it from the tutorial
//#ifdef WITH_PICK_HANDLER
//	view->addEventHandler(new PickHandler);
//#endif

	osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
	manipulator->setAllowThrow(false);

	viewer_->setCameraManipulator(manipulator);

	// for composite viewer
	// viewer_->addView(view);
	// viewer_->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
	// viewer_->realize();

	// This ensures that the widget will receive keyboard events. This focus
	// policy is not set by default. The default, Qt::NoFocus, will result in
	// keyboard events that are ignored.
	this->setFocusPolicy(Qt::StrongFocus);
	this->setMinimumSize(100, 100);

	// Ensures that the widget receives mouse move events even though no
	// mouse button has been pressed. We require this in order to let the
	// graphics window switch viewports properly.
	this->setMouseTracking(true);
}

osgViewer::Viewer * OSGViewerWidget::GetViewer()
{
	return viewer_;
}

void OSGViewerWidget::paintEvent(QPaintEvent* /* paintEvent */)
{
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
		osgDB::writeNodeFile(*viewer_->getSceneData(),
			"/tmp/sceneGraph.osg");

		return;
	}
	else if (event->key() == Qt::Key_H)
	{
		this->onHome();
		return;
	}

	this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol(*keyData));
}

void OSGViewerWidget::keyReleaseEvent(QKeyEvent* event)
{
	QString keyString = event->text();
	const char* keyData = keyString.toLocal8Bit().data();

	this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*keyData));
}

void OSGViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
	// Note that we have to check the buttons mask in order to see whether the
	// left button has been pressed. A call to `button()` will only result in
	// `Qt::NoButton` for mouse move events.
	if (selectionActive_ && event->buttons() & Qt::LeftButton)
	{
		selectionEnd_ = event->pos();

		// Ensures that new paint events are created while the user moves the
		// mouse.
		this->update();
	}
	else
	{
		this->getEventQueue()->mouseMotion(static_cast<float>(event->x()),
			static_cast<float>(event->y()));
	}
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
