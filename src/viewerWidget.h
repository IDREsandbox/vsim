#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QtCore/QObject>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/CompositeViewer>
#include <osgQt/GraphicsWindowQt>
#include <osg/ShapeDrawable>
#include <osg/Material>

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>

class viewerWidget : public QWidget, public osgViewer::CompositeViewer
{
	Q_OBJECT

public:

	viewerWidget(QMainWindow *parent)
	: QWidget(parent)
	{
		setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

		//gw = createGraphicsWindow(0, 0, 640, 480);

		osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		traits->windowDecoration = false;
		traits->x = 0;
		traits->y = 0;
		traits->width = 640;
		traits->height = 480;
		traits->doubleBuffer = true;
		traits->alpha = ds->getMinimumNumAlphaBits();
		traits->stencil = ds->getMinimumNumStencilBits();
		traits->sampleBuffers = ds->getMultiSamples();
		traits->samples = ds->getNumMultiSamples();

		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		camera->setGraphicsContext(new osgQt::GraphicsWindowQt(traits.get()));

		camera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
		camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
		camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0f, 10000.0f);

		osg::Node* scene = osgDB::readNodeFile("T:\\Projects\\_UCLA\\vsim\\vsim-dependencies\\OpenSceneGraph-Data-3.4.0\\cow.osg");

		view = new osgViewer::View;
		view->setCamera(camera);
		addView(view);
		view->setSceneData(scene);
		view->addEventHandler(new osgViewer::StatsHandler);
		view->setCameraManipulator(new osgGA::TrackballManipulator);
		osgQt::GraphicsWindowQt* gw = dynamic_cast<osgQt::GraphicsWindowQt*>(camera->getGraphicsContext());

		//const osg::GraphicsContext::Traits* traits = gw->getTraits();

		/*osg::Camera* camera = _viewer.getCamera();
		camera->setGraphicsContext(gw);
		camera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
		camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
		camera->setProjectionMatrixAsPerspective(
			30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0f, 10000.0f);

		_viewer.setSceneData(scene);
		_viewer.addEventHandler(new osgViewer::StatsHandler);
		_viewer.setCameraManipulator(new osgGA::TrackballManipulator);
		_viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);*/

		//QGridLayout* layout = new QGridLayout(this);

		QVBoxLayout* back = new QVBoxLayout(this);
		//back->setContentsMargins(0, 0, 0, 0);
		QWidget* qScene = reinterpret_cast<QWidget*>(gw->getGLWidget());
		//qScene->setContentsMargins(0, 0, 0, 0);
		qScene->setParent(this);
		back->addWidget(qScene);
		//back->setContentsMargins(0, 0, 0, 0);
		setLayout(back);

		//QPushButton* button = new QPushButton(this);
		//QObject::connect(button, SIGNAL(clicked()), &app, SLOT(quit()));
		//button->setMinimumSize(140, 140);

		//layout->addWidget(button, 2, 0);

		//setLayout(back);

		connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
		_timer.start(10);

		//_thread.viewerPtr = &_viewer;
		//_thread.start();
	}

	void setNewScene(osg::Node* scene){
		view->setSceneData(scene);
	}

	//osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h)
	//{
	//	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	//	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	//	traits->windowDecoration = false;
	//	traits->x = x;
	//	traits->y = y;
	//	traits->width = w;
	//	traits->height = h;
	//	traits->doubleBuffer = true;
	//	traits->alpha = ds->getMinimumNumAlphaBits();
	//	traits->stencil = ds->getMinimumNumStencilBits();
	//	traits->sampleBuffers = ds->getMultiSamples();
	//	traits->samples = ds->getNumMultiSamples();

	//	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	//	camera->setGraphicsContext(new osgQt::GraphicsWindowQt(traits.get()));
	//	
	//	camera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
	//	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	//	camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0f, 10000.0f);

	//	osgViewer::View* view = new osgViewer::View;
	//	view->setCamera(camera);
	//	addView(view);
	//	view->setSceneData(scene);
	//	view->addEventHandler(new osgViewer::StatsHandler);
	//	view->setCameraManipulator(new osgGA::TrackballManipulator);
	//	GraphicsWindowQt* gw = dynamic_cast<GraphicsWindowQt*>(camera->getGraphicsContext());

	//	return new osgQt::GraphicsWindowQt(traits.get());
	//}

	//osgQt::GraphicsWindowQt* gw;
	virtual void paintEvent(QPaintEvent* event)
	{
		frame();
	}

protected:
	/*virtual void resizeEvent(QResizeEvent *event)
	{
	std::vector<osg::Camera*> cameras;
	_viewer.getCameras(cameras);

	cameras[0]->setViewport(0, 0, this->width() / 2, this->height());
	}*/
	

	osgViewer::View* view;
	//RenderThread _thread;
	QTimer _timer;
	
};

#endif // VIEWERWIDGET_H