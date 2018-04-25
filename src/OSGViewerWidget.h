// Credits and thanks to Bastian Rieck for fitting VSim with Qt http://bastian.rieck.ru/blog/posts/2014/qt_and_openscenegraph/
// Small adaptations were made for VSim purposes

// This is a Qt Widget with an openscenegraph GraphicsWindowEmbedded + viewer inside

#ifndef OSGVIEWERWIDGET_H
#define OSGVIEWERWIDGET_H

#include <QOpenGLWidget>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osg/Camera>
#include <QElapsedTimer>

class SimpleCameraManipulator;
class FirstPersonManipulator;
class FlightManipulator;
class ObjectManipulator;
class KeyTracker;

#include "Navigation.h"

enum Manipulator {
	MANIPULATOR_SIMPLE, // does nothing
	MANIPULATOR_FIRST_PERSON,
	MANIPULATOR_FLIGHT,
	MANIPULATOR_OBJECT
};

class OSGViewerWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	OSGViewerWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

	// osgViewer::Viewer* setViewer(osgViewer::Viewer*); // can't change the viewer
	osgViewer::ViewerBase *getViewer() const;
	osgViewer::View *mainView() const;

	osg::Matrixd getCameraMatrix();
	void setCameraMatrix(osg::Matrixd);

	// sets navigation mode and unfreezes
	void setNavigationMode(Navigation::Mode);
	Navigation::Mode getNavigationMode() const;

	void setManipulator(Manipulator manipulator);
	Manipulator getManipulator() const; // if frozen returns MANIPULATOR_SIMPLE

	static Navigation::Mode manipulatorToNavigation(Manipulator);
	static Manipulator navigationToManipulator(Navigation::Mode);

	void adjustSpeed(int tick);

	void setCameraFrozen(bool freeze);
	bool getCameraFrozen() const;
	void figureOutNavigation();

	void stopNavigation();

	void flightStartStrafe();
	void flightStopStrafe();

	void enableGravity(bool enable);
	void enableCollisions(bool enable);

	void enableRendering(bool enable);
	void setViewer(osgViewer::CompositeViewer *viewer);

	void reset();

	// TODO? Just use QWidget::Render
	// QImage takePictureAt(osg::Camera*);

	// filter out ctrl-s when in wasd
	bool eventFilter(QObject *obj, QEvent *e);

	QImage renderView(QSize size, const osg::Matrixd &matrix);

	float getFrameTime() const;
	float getTimeBetween() const;
	float getFullFrameTime() const;

signals:
	void frame(double dt_sec);

	void sNavigationModeChanged(Navigation::Mode);
	void sCameraFrozen(bool);

protected:
	virtual void paintEvent(QPaintEvent* paintEvent) override;
	virtual void paintGL() override;
	virtual void resizeGL(int width, int height) override;

	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void keyReleaseEvent(QKeyEvent* event) override;

	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;

	virtual void focusOutEvent(QFocusEvent *event) override;
	virtual void focusInEvent(QFocusEvent *event) override;

	virtual bool event(QEvent* event) override;

private:
	virtual void onHome();
	//virtual void onResize(int width, int height);
	
	// used for first person mode
	void takeCursor();
	void releaseCursor();
	void centerCursor();

	osgGA::EventQueue* getEventQueue() const;

	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow_;
	osg::ref_ptr<osgViewer::CompositeViewer> m_viewer;

	osg::ref_ptr<osgViewer::View> m_main_view;

	// camera and viewer stuff
	// camera manipulators
	Manipulator m_manipulator;
	Navigation::Mode m_navigation_mode;
	// NavigationMode m_navigation_mode;
	osg::ref_ptr<SimpleCameraManipulator> m_simple_manipulator; // this one has no controls	
	osg::ref_ptr<FirstPersonManipulator> m_first_person_manipulator;
	osg::ref_ptr<FlightManipulator> m_flight_manipulator;
	osg::ref_ptr<ObjectManipulator> m_object_manipulator;

	bool m_camera_frozen;
	bool m_gravity_on;
	bool m_collisions_on;
	int m_speed_tick;

	bool m_rendering_enabled;

	// key press tracker
	KeyTracker *m_key_tracker;

	QElapsedTimer m_frame_timer;
	QElapsedTimer m_between_timer;
	float m_frame_time;
	float m_full_frame_time;
	float m_time_between;
};
#endif