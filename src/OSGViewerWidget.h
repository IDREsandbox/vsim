// Credits and thanks to Bastian Rieck for fitting VSim with Qt http://bastian.rieck.ru/blog/posts/2014/qt_and_openscenegraph/
// Small adaptations were made for VSim purposes

// This is a Qt Widget with an openscenegraph GraphicsWindowEmbedded + viewer inside

#ifndef OSGVIEWERWIDGET_H
#define OSGVIEWERWIDGET_H

#include <QOpenGLWidget>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osg/Camera>
#include <QElapsedTimer>
#include "SimpleCameraManipulator.h"
#include "FirstPersonManipulator.h"
#include "FlightManipulator.h"
#include "ObjectManipulator.h"
#include "KeyTracker.h"

class OSGViewerWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	OSGViewerWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

	// osgViewer::Viewer* setViewer(osgViewer::Viewer*); // can't change the viewer
	osgViewer::Viewer* getViewer() const;

	osg::Matrixd getCameraMatrix();
	void setCameraMatrix(osg::Matrixd);

	enum NavigationMode {
		NAVIGATION_SIMPLE, // does nothing, (used for playing/pausing)
		NAVIGATION_FIRST_PERSON,
		NAVIGATION_FLIGHT,
		NAVIGATION_OBJECT
	};

	void setNavigationMode(NavigationMode);
	NavigationMode getNavigationMode() const;
	NavigationMode getActualNavigationMode() const; // if frozen returns NAVIGATION_SIMPLE

	void setCameraFrozen(bool freeze);
	bool getCameraFrozen() const;

	void reset();

	// TODO? Just use QWidget::Render
	// QImage takePictureAt(osg::Camera*);

protected:
	virtual void paintEvent(QPaintEvent* paintEvent);
	virtual void paintGL();
	virtual void resizeGL(int width, int height);

	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);

	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

	virtual bool event(QEvent* event);

private:
	virtual void onHome();
	virtual void onResize(int width, int height);
	
	// used for first person mode
	void takeCursor();
	void releaseCursor();
	void centerCursor();

	osgGA::EventQueue* getEventQueue() const;

	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow_;
	osg::ref_ptr<osgViewer::Viewer> viewer_;

	// camera and viewer stuff
	// camera manipulators
	NavigationMode m_navigation_mode;
	osg::ref_ptr<SimpleCameraManipulator> m_simple_manipulator; // this one has no controls	
	osg::ref_ptr<FirstPersonManipulator> m_first_person_manipulator;
	osg::ref_ptr<FlightManipulator> m_flight_manipulator;
	osg::ref_ptr<ObjectManipulator> m_object_manipulator;

	bool m_camera_frozen;
	bool m_gravity_on;
	bool m_collisions_on;

	// key press tracker
	KeyTracker m_key_tracker;

	QElapsedTimer m_frame_timer;
};
#endif