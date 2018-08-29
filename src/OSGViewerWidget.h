// Credits and thanks to Bastian Rieck for fitting VSim with Qt http://bastian.rieck.ru/blog/posts/2014/qt_and_openscenegraph/
// Small adaptations were made for VSim purposes

// This is a Qt Widget with an openscenegraph GraphicsWindowEmbedded + viewer inside

#ifndef OSGVIEWERWIDGET_H
#define OSGVIEWERWIDGET_H

#include <QOpenGLWidget>
#include <QAction>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/CompositeViewer>
#include <osg/Camera>
#include <osgGA/StateSetManipulator>
#include <QElapsedTimer>
#include <QOpenGLFramebufferObject>
#include <memory>
#include <queue>

#include "Navigation.h"

class SimpleCameraManipulator;
class FirstPersonManipulator;
class FlightManipulator;
class ObjectManipulator;
class KeyTracker;
class FBOGraphicsWindow;

enum Manipulator {
	MANIPULATOR_SIMPLE, // does nothing
	MANIPULATOR_FIRST_PERSON,
	MANIPULATOR_FLIGHT,
	MANIPULATOR_OBJECT
};

enum LengthUnit {
	Meters,
	Centimeters,
	Feet
};

struct ViewerPreset {
	const char *name;
	LengthUnit unit;
	float base_speed;
	int speed_tick;
	float collision_radius;
	float eye_height;
	float gravity_acceleration; // positive
	float gravity_fall_speed;
};

class OSGViewerWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	OSGViewerWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~OSGViewerWidget() override;

	// osgViewer::Viewer* setViewer(osgViewer::Viewer*); // can't change the viewer
	osgViewer::ViewerBase *getViewer() const;
	osgViewer::View *mainView() const;

	void setSceneData(osg::Node *scene);

	osgGA::StateSetManipulator *getStateSetManipulator();
	void cycleStats();
	//osgViewer::StatsHandler *getStatsHandler();

	osg::Matrixd getCameraMatrix();
	void setCameraMatrix(osg::Matrixd);

	// sets navigation mode and unfreezes
	void setNavigationMode(Navigation::Mode);
	Navigation::Mode getNavigationMode() const;

	void setManipulator(Manipulator manipulator);
	Manipulator getManipulator() const; // if frozen returns MANIPULATOR_SIMPLE

	static Navigation::Mode manipulatorToNavigation(Manipulator);
	static Manipulator navigationToManipulator(Navigation::Mode);

	void setCameraFrozen(bool freeze);
	bool getCameraFrozen() const;
	void figureOutNavigation();

	void stopNavigation();

	void flightStartStrafe();
	void flightStopStrafe();

	// thumbnails
	// Queues up a thumbnail to be painted.
	// When it's ready sThumbnailFinished(QImage img) is emitted
	// Can only paint one thumbnail at a time, returns false if rejected
	// TODO: awkward design, only one can use this at a time,
	//       refactor with a done callback? add queue?
	bool paintThumbnail(const osg::Matrix &camera);
	void setThumbnailSize(QSize size);

	int samples();
	void setSamples(int samples);

	void enableRendering(bool enable);
	//void setViewer(osgViewer::CompositeViewer *viewer);

	// resets camera
	// applies startups (groundOnStartup)
	void startup();

	// properties

	void enableGroundMode(bool enable);
	bool groundModeEnabled() const;
	bool groundOnStartup() const;
	void setGroundOnStartup(bool ground);

	bool collisionsEnabled() const;
	void enableCollisions(bool enable);
	bool collisionOnStartup() const;
	void setCollisionOnStartup(bool ground);

	QAction *a_speed_up;
	QAction *a_slow_down;
	static const int k_tick_limit = 40;
	void adjustSpeed(int tick);
	int speedTick() const;
	void setSpeedTick(int tick);
	int startupSpeedTick() const;
	void setStartupSpeedTick(int tick);
	static float speedMultiplier(int tick);

	float fovy() const;
	void setFovy(float fovy);

	bool autoClip() const;
	void setAutoClip(bool auto_clip);

	float nearClip() const;
	void setNearClip(float near_clip);

	float farClip() const;
	void setFarClip(float far_clip);

	float eyeHeight() const;
	void setEyeHeight(float height) const;

	// usually set to negative
	float gravityAcceleration() const;
	void setGravityAcceleration(float accel);

	float gravitySpeed() const;
	void setGravitySpeed(float speed);

	float baseSpeed() const;
	void setBaseSpeed(float speed);

	float collisionRadius() const;
	void setCollisionRadius(float radius);

	LengthUnit lengthUnit() const;
	void setLengthUnit(LengthUnit unit);

	// sets the unit and loads the defaults for that unit (ex. 1.7m tall)
	//void setUnitPreset(const ViewerPreset &preset);
	const std::vector<ViewerPreset> &presets() const;

	void goHome();
	osg::Matrix defaultHomePosition() const;
	osg::Matrix homePosition() const;
	void setHomePosition(const osg::Matrix &camera_matrix);
	void resetHomePosition();
	bool usingDefaultHomePosition();

	// state set manipulator wrapper
	bool getLightingEnabled() const;
	bool getBackfaceEnabled() const;
	bool getTextureEnabled() const;
	osg::PolygonMode::Mode getPolygonMode() const;
	void setLightingEnabled(bool enable);
	void setBackfaceEnabled(bool enable);
	void setTextureEnabled(bool enable);
	void setPolygonMode(osg::PolygonMode::Mode mode);


public:

	// filter out ctrl-s when in wasd
	bool eventFilter(QObject *obj, QEvent *e);

	float getFrameTime() const;
	float getTimeBetween() const;
	float getFullFrameTime() const;
	QString debugString();

signals:
	void frame(double dt_sec);

	void sNavigationModeChanged(Navigation::Mode);
	void sCameraFrozen(bool);

	void sThumbnailFinished(QImage img);

	// properties
	void sSpeedActivelyChanged(int tick, double multiplier);
	void sCollisionsChanged(bool collisions);
	void sGroundModeChanged(bool ground);

protected:
	virtual void paintEvent(QPaintEvent* paintEvent) override;
	void initializeGL() override;
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
	//virtual void onHome(); // osg event, ignore it
	//virtual void onResize(int width, int height);
	
	// used for first person mode
	void takeCursor();
	void releaseCursor();
	void centerCursor();

	void recreateFramebuffer();

private:
	osgGA::EventQueue* getEventQueue() const;

	osg::ref_ptr<osgViewer::GraphicsWindow> m_shared_graphics_window;
	osg::ref_ptr<FBOGraphicsWindow> m_graphics_window;
	osg::ref_ptr<FBOGraphicsWindow> m_thumb_graphics_window;

	//osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow_;
	osg::ref_ptr<osgViewer::CompositeViewer> m_viewer;

	osg::ref_ptr<osgViewer::View> m_main_view;
	osg::ref_ptr<osgViewer::View> m_thumb_view;

	std::shared_ptr<QOpenGLFramebufferObject> m_fbo;
	std::shared_ptr<QOpenGLFramebufferObject> m_thumb_fbo;

	QSize m_thumb_size;
	bool m_paint_thumb;

	// camera and viewer stuff
	// camera manipulators
	Manipulator m_manipulator;
	Navigation::Mode m_navigation_mode;
	// NavigationMode m_navigation_mode;
	osg::ref_ptr<SimpleCameraManipulator> m_simple_manipulator; // this one has no controls	
	osg::ref_ptr<FirstPersonManipulator> m_first_person_manipulator;
	osg::ref_ptr<FlightManipulator> m_flight_manipulator;
	osg::ref_ptr<ObjectManipulator> m_object_manipulator;

	osg::ref_ptr<osgGA::StateSetManipulator> m_ssm;
	osg::ref_ptr<osgGA::StateSetManipulator> m_thumb_ssm;
	osg::ref_ptr<osgViewer::StatsHandler> m_stats;

	bool m_camera_frozen;
	int m_speed_tick;
	int m_startup_speed_tick;
	bool m_rendering_enabled;
	osg::Matrix m_home_position;
	bool m_default_home_position;
	bool m_collision_on_startup;
	bool m_ground_on_startup;
	LengthUnit m_length_unit;
	std::vector<ViewerPreset> m_presets;

	// key press tracker
	KeyTracker *m_key_tracker;

	QElapsedTimer m_frame_timer;
	QElapsedTimer m_between_timer;
	float m_frame_time;
	float m_full_frame_time;
	float m_time_between;

	int m_samples;
};
#endif