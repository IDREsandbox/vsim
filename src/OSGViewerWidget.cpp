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


class DummyGraphicsWindow : public osgViewer::GraphicsWindowEmbedded
{
public:

	DummyGraphicsWindow(int x, int y, int width, int height)
		: GraphicsWindowEmbedded(x, y, width, height)
	{
	}

	bool makeCurrentImplementation() override {
		return osgViewer::GraphicsWindowEmbedded::makeCurrentImplementation();
	}
	bool releaseContextImplementation() override {
		return osgViewer::GraphicsWindowEmbedded::releaseContextImplementation();
	}
};


class FBOGraphicsWindow : public osgViewer::GraphicsWindowEmbedded
{
public:

	FBOGraphicsWindow(osg::GraphicsContext::Traits *traits)
		: GraphicsWindowEmbedded(traits)
	{
	}

	void setFramebuffer(std::weak_ptr<QOpenGLFramebufferObject> fbo) {
		m_fbo = fbo;
	}

	bool makeCurrentImplementation() override {
		auto fbo = m_fbo.lock();
		if (fbo) {
			fbo->bind();
		}
		return osgViewer::GraphicsWindowEmbedded::makeCurrentImplementation();
	}
	bool releaseContextImplementation() override {
		return osgViewer::GraphicsWindowEmbedded::releaseContextImplementation();
	}

private:
	std::weak_ptr<QOpenGLFramebufferObject> m_fbo;
};


OSGViewerWidget::OSGViewerWidget(QWidget* parent, Qt::WindowFlags f)
	: QOpenGLWidget(parent,	f),
	m_rendering_enabled(true),
	m_paint_thumb(false),
	m_thumb_size(100, 100),
	m_collision_on_startup(true),
	m_ground_on_startup(false),
	m_default_home_position(true),
	m_startup_speed_tick(0)
{
	// Qt Stuff
	// Anti aliasing (QOpenGLWidget)
	QSurfaceFormat fmt;
	fmt.setSamples(0);
	setFormat(fmt);
	m_samples = 4;

	// need a context for this, do it later
	//recreateFramebuffer();

	// Create viewer, graphics context, and link them together

	m_viewer = new osgViewer::CompositeViewer();
	m_viewer->setThreadingModel(osgViewer::ViewerBase::ThreadingModel::SingleThreaded);

	m_main_view = new osgViewer::View();
	m_viewer->addView(m_main_view);
	m_thumb_view = new osgViewer::View();

	m_shared_graphics_window = new DummyGraphicsWindow(100, 100, 100, 100);

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(osg::DisplaySettings::instance());
	traits->x = x();
	traits->y = y();
	traits->width = width();
	traits->height = height();
	traits->sharedContext = m_shared_graphics_window.get();
	m_graphics_window = new FBOGraphicsWindow(traits);
	m_graphics_window->setName("main_context");

	osg::ref_ptr<osg::GraphicsContext::Traits> traits2 = new osg::GraphicsContext::Traits(osg::DisplaySettings::instance());
	traits2->x = 0;
	traits2->y = 0;
	traits2->width = m_thumb_size.width();
	traits2->height = m_thumb_size.height();
	traits2->sharedContext = m_shared_graphics_window.get();
	m_thumb_graphics_window = new FBOGraphicsWindow(traits2);
	m_thumb_graphics_window->setName("thumb_context");

	float aspect_ratio = this->width() / (float)this->height();

	// Create an empty scene
	osg::Group* group = new osg::Group();
	setSceneData(group);

	// Use default camera
	osg::Camera* camera = m_main_view->getCamera();
	float cfar = 7500.0f;
	float cnear = 1.0f;
	camera->setViewport(0, 0, this->width(), this->height());
	camera->setClearColor(osg::Vec4(51/255.f, 51/255.f, 102/255.f, 1.f));
	camera->setProjectionMatrixAsPerspective(55.f, aspect_ratio, cnear, cfar);
	camera->setGraphicsContext(m_graphics_window); // set the context, connects the viewer and graphics context
	//camera->setComputeNearFarMode(osg::CullSettings::ComputeNearFarMode::DO_NOT_COMPUTE_NEAR_FAR);
	camera->setComputeNearFarMode(osg::CullSettings::ComputeNearFarMode::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	//camera->setCullingMode(osg::Camera::NO_CULLING);

	osg::Camera* camera2 = m_thumb_view->getCamera();
	camera2->setViewport(0, 0, 100, 100);
	camera2->setClearColor(osg::Vec4(51 / 255.f, 51 / 255.f, 102 / 255.f, 1.f));
	camera2->setProjectionMatrixAsPerspective(55.f, m_thumb_size.width() / m_thumb_size.height(), 1.f, 7500.f);
	camera2->setGraphicsContext(m_thumb_graphics_window);
	m_thumb_view->setCamera(camera2);

	// Stats Handler
	m_stats = new osgViewer::StatsHandler;
	m_stats->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F30); // biggest hack
	m_stats->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_F29);
	m_main_view->addEventHandler(m_stats);

	// Camera State Handler
	m_ssm = new osgGA::StateSetManipulator(camera->getOrCreateStateSet());
	//ssm->setKeyEventToggleBackfaceCulling('B');
	//ssm->setKeyEventToggleLighting('L');
	//ssm->setKeyEventToggleTexturing('X');
	//ssm->setKeyEventCyclePolygonMode('M');
	//m_main_view->addEventHandler(ssm);
	//auto ss = m_main_view->getCamera()->getStateSet();

	// thumbnail stateset
	m_thumb_ssm = new osgGA::StateSetManipulator(camera2->getOrCreateStateSet());

	setLightingEnabled(true);
	setBackfaceEnabled(false);
	setTextureEnabled(true);
	setPolygonMode(osg::PolygonMode::FILL);

	// Lighting
	// share the same light
	m_main_view->setLightingMode(osg::View::LightingMode::HEADLIGHT);
	auto *light = m_main_view->getLight();
	m_thumb_view->setLightingMode(osg::View::LightingMode::HEADLIGHT);
	m_thumb_view->setLight(light);

	light->setAmbient(osg::Vec4(.5, .5, .5, 1.0));

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

	// speed
	a_speed_up = new QAction(this);
	a_speed_up->setText("Speed Up");
	a_speed_up->setShortcut(QKeySequence(Qt::Key_Equal));
	addAction(a_speed_up);
	connect(a_speed_up, &QAction::triggered, this, [this]() {
		adjustSpeed(1);
		emit sSpeedActivelyChanged(m_speed_tick,
			m_flight_manipulator->getSpeedMultiplier());
	});

	a_slow_down = new QAction(this);
	a_slow_down->setText("Slow Down");
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

	// presets and units
	ViewerPreset meters, cm, feet;
	meters.unit = Meters;
	meters.name = "meters";
	meters.base_speed = 10.0;
	meters.speed_tick = 0;
	meters.collision_radius = .7;
	meters.eye_height = 1.7;
	meters.gravity_acceleration = -10.0;
	meters.gravity_fall_speed = 40.0;

	auto scalePreset = [](const ViewerPreset &base, float scale, ViewerPreset *out) {
		out->base_speed = base.base_speed * scale;
		out->speed_tick = base.speed_tick;
		out->collision_radius = base.collision_radius * scale;
		out->eye_height = base.eye_height * scale;
		out->gravity_acceleration = base.gravity_acceleration * scale;
		out->gravity_fall_speed = base.gravity_fall_speed * scale;
	};

	cm.name = "cm";
	cm.unit = Centimeters;
	scalePreset(meters, 100.0, &cm);

	feet.name = "feet";
	feet.unit = Feet;
	scalePreset(meters, 3.0, &feet);

	m_presets.push_back(meters);
	m_presets.push_back(cm);
	m_presets.push_back(feet);

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

	//
	connect(this, &QOpenGLWidget::frameSwapped, this, [this]() {
		if (m_paint_thumb) {
			m_paint_thumb = false;
			QImage img = m_thumb_fbo->toImage();
			m_viewer->removeView(m_thumb_view);
			// grab the thumbnail
			emit sThumbnailFinished(img);
		}
	});
}

OSGViewerWidget::~OSGViewerWidget()
{
}

osgViewer::ViewerBase* OSGViewerWidget::getViewer() const
{
	return m_viewer;
}

osgViewer::View * OSGViewerWidget::mainView() const
{
	return m_main_view;
}

void OSGViewerWidget::setSceneData(osg::Node * scene)
{
	m_main_view->setSceneData(scene);
	m_thumb_view->setSceneData(scene);
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

bool OSGViewerWidget::groundModeEnabled() const
{
	return m_first_person_manipulator->gravityEnabled();
}

void OSGViewerWidget::enableGroundMode(bool enable)
{
	m_flight_manipulator->enableGravity(enable);
	m_first_person_manipulator->enableGravity(enable);
	m_flight_manipulator->enableHeight(enable);
	m_first_person_manipulator->enableHeight(enable);
	emit sGroundModeChanged(enable);
}

bool OSGViewerWidget::collisionsEnabled() const
{
	return m_first_person_manipulator->collisionsEnabled();
}

void OSGViewerWidget::enableCollisions(bool enable)
{
	m_flight_manipulator->enableCollisions(enable);
	m_first_person_manipulator->enableCollisions(enable);
	emit sCollisionsChanged(enable);
}

bool OSGViewerWidget::paintThumbnail(const osg::Matrix & camera)
{
	if (m_paint_thumb) {
		qWarning() << "Can only paint one thumbnail at a time";
		return false;
	}
	m_paint_thumb = true;
	m_viewer->addView(m_thumb_view);

	osg::Matrix view_matrix = osg::Matrix::inverse(camera);
	m_thumb_view->getCamera()->setViewMatrix(view_matrix);
	return true;
}

int OSGViewerWidget::samples()
{
	return m_samples;
}

void OSGViewerWidget::setSamples(int samples)
{
	m_samples = samples;
	recreateFramebuffer();
}

void OSGViewerWidget::enableRendering(bool enable)
{
	m_rendering_enabled = enable;
	if (enable) update();
}

void OSGViewerWidget::startup()
{
	setNavigationMode(Navigation::OBJECT);
	enableCollisions(m_collision_on_startup);
	enableGroundMode(m_ground_on_startup);
	goHome();
}

void OSGViewerWidget::adjustSpeed(int tick)
{
	int speed_limit = k_tick_limit;
	m_speed_tick += tick;
	m_speed_tick = std::clamp(m_speed_tick, -speed_limit, speed_limit);
	m_first_person_manipulator->setSpeedTick(m_speed_tick);
	m_flight_manipulator->setSpeedTick(m_speed_tick);
}

int OSGViewerWidget::speedTick() const
{
	return m_speed_tick;
}

void OSGViewerWidget::setSpeedTick(int tick)
{
	m_speed_tick = tick;
}

int OSGViewerWidget::startupSpeedTick() const
{
	return m_startup_speed_tick;
}

void OSGViewerWidget::setStartupSpeedTick(int tick)
{
	m_startup_speed_tick = tick;
}

float OSGViewerWidget::speedMultiplier(int tick)
{
	return BaseFirstPersonManipulator::speedMultiplierForTick(tick);
}

float OSGViewerWidget::fovy() const
{
	double fovy, aspect, near_clip, far_clip;
	m_main_view->getCamera()->getProjectionMatrixAsPerspective(fovy, aspect, near_clip, far_clip);
	return fovy;
}

void OSGViewerWidget::setFovy(float fovy2)
{
	double fovy, aspect, near_clip, far_clip;
	m_main_view->getCamera()->getProjectionMatrixAsPerspective(fovy, aspect, near_clip, far_clip);
	m_main_view->getCamera()->setProjectionMatrixAsPerspective(fovy2, aspect, near_clip, far_clip);
	m_thumb_view->getCamera()->setProjectionMatrixAsPerspective(fovy2, aspect, near_clip, far_clip);
}

bool OSGViewerWidget::autoClip() const
{
	auto setting = m_main_view->getCamera()->getComputeNearFarMode();
	if (setting == osg::CullSettings::ComputeNearFarMode::DO_NOT_COMPUTE_NEAR_FAR) {
		return false;
	}
	return true;
}

void OSGViewerWidget::setAutoClip(bool auto_clip)
{
	auto setting = auto_clip ?
		osg::CullSettings::ComputeNearFarMode::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES
		: osg::CullSettings::ComputeNearFarMode::DO_NOT_COMPUTE_NEAR_FAR;

	m_main_view->getCamera()->setComputeNearFarMode(setting);
	m_thumb_view->getCamera()->setComputeNearFarMode(setting);
}

float OSGViewerWidget::nearClip() const
{
	double a, b, near_clip, far_clip;
	m_main_view->getCamera()->getProjectionMatrixAsPerspective(a, b, near_clip, far_clip);
	return near_clip;
}

void OSGViewerWidget::setNearClip(float near_clip_2)
{
	double a, b, near_clip, far_clip;
	m_main_view->getCamera()->getProjectionMatrixAsPerspective(a, b, near_clip, far_clip);

	// make sure far > near > min
	double near_min = .001; // near > min
	double factor_min = 2; // far > 2*near

	near_clip_2 = std::max({ (double)near_clip_2, near_min });
	double far_clip_2 = std::max({ far_clip, near_clip_2 * factor_min, near_min });

	m_main_view->getCamera()->setProjectionMatrixAsPerspective(a, b, near_clip_2, far_clip_2);
	m_thumb_view->getCamera()->setProjectionMatrixAsPerspective(a, b, near_clip_2, far_clip_2);
}

float OSGViewerWidget::farClip() const
{
	double a, b, near_clip, far_clip;
	m_main_view->getCamera()->getProjectionMatrixAsPerspective(a, b, near_clip, far_clip);
	return far_clip;
}

void OSGViewerWidget::setFarClip(float far_clip_2)
{
	double a, b, near_clip, far_clip;
	m_main_view->getCamera()->getProjectionMatrixAsPerspective(a, b, near_clip, far_clip);

	// make sure far > near > min
	double near_min = .001;
	double factor_min = 2; // far > 2*near

	// new near clip too small?
	double near_clip_2 = std::min(near_clip, far_clip_2 / factor_min);
	if (near_clip_2 < near_min) {
		setNearClip(near_min);
		return;
	}

	m_main_view->getCamera()->setProjectionMatrixAsPerspective(a, b, near_clip_2, far_clip_2);
	m_thumb_view->getCamera()->setProjectionMatrixAsPerspective(a, b, near_clip_2, far_clip_2);
}

float OSGViewerWidget::eyeHeight() const
{
	return m_first_person_manipulator->eyeHeight();
}

void OSGViewerWidget::setEyeHeight(float height) const
{
	m_first_person_manipulator->setEyeHeight(height);
	m_flight_manipulator->setEyeHeight(height);
}

float OSGViewerWidget::gravityAcceleration() const
{
	return m_first_person_manipulator->gravityAcceleration();
}

void OSGViewerWidget::setGravityAcceleration(float accel)
{
	m_first_person_manipulator->setGravityAcceleration(accel);
	m_flight_manipulator->setGravityAcceleration(accel);
}

float OSGViewerWidget::gravitySpeed() const
{
	return m_first_person_manipulator->gravitySpeed();
}

void OSGViewerWidget::setGravitySpeed(float speed)
{
	m_first_person_manipulator->setGravitySpeed(speed);
	m_flight_manipulator->setGravitySpeed(speed);
}

float OSGViewerWidget::baseSpeed() const
{
	return m_first_person_manipulator->baseSpeed();
}

void OSGViewerWidget::setBaseSpeed(float speed)
{
	m_first_person_manipulator->setBaseSpeed(speed);
	m_flight_manipulator->setStrafeSpeed(speed);
	m_flight_manipulator->setAcceleration(speed / 2);
}

float OSGViewerWidget::collisionRadius() const
{
	return m_first_person_manipulator->collisionRadius();
}

void OSGViewerWidget::setCollisionRadius(float radius)
{
	m_first_person_manipulator->setCollisionRadius(radius);
	m_flight_manipulator->setCollisionRadius(radius);
}

LengthUnit OSGViewerWidget::lengthUnit() const
{
	return m_length_unit;
}

void OSGViewerWidget::setLengthUnit(LengthUnit unit)
{
	m_length_unit = unit;
}

//void OSGViewerWidget::setUnitPreset(const ViewerPreset & preset)
//{
//	setBaseSpeed(preset.base_speed);
//	setSpeedTick(preset.speed_tick);
//	setStartupSpeedTick(preset.speed_tick);
//	setCollisionRadius(preset.collision_radius);
//	setEyeHeight(preset.eye_height);
//	setGravityAcceleration(preset.gravity_acceleration);
//	setCollisionRadius(preset.collision_radius)
//}

const std::vector<ViewerPreset>& OSGViewerWidget::presets() const
{
	return m_presets;
}

bool OSGViewerWidget::groundOnStartup() const
{
	return m_ground_on_startup;
}

void OSGViewerWidget::setGroundOnStartup(bool ground)
{
	m_ground_on_startup = ground;
}

bool OSGViewerWidget::collisionOnStartup() const
{
	return m_collision_on_startup;
}

void OSGViewerWidget::setCollisionOnStartup(bool ground)
{
	m_collision_on_startup = ground;
}

void OSGViewerWidget::goHome()
{
	setCameraMatrix(homePosition());
}

osg::Matrix OSGViewerWidget::defaultHomePosition() const
{
	// pretty much copied from osgGA::CameraManipulator::computeHomePosition
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

	return osg::Matrix::inverse(mat);
}

osg::Matrix OSGViewerWidget::homePosition() const
{
	if (m_default_home_position) {
		return defaultHomePosition();
	}
	else return m_home_position;
}

void OSGViewerWidget::setHomePosition(const osg::Matrix & camera_matrix)
{
	m_home_position = camera_matrix;
	m_default_home_position = false;
}

void OSGViewerWidget::resetHomePosition()
{
	m_default_home_position = true;
}

bool OSGViewerWidget::usingDefaultHomePosition()
{
	return m_default_home_position;
}

bool OSGViewerWidget::getLightingEnabled() const
{
	return m_ssm->getLightingEnabled();
}

bool OSGViewerWidget::getBackfaceEnabled() const
{
	return m_ssm->getBackfaceEnabled();
}

bool OSGViewerWidget::getTextureEnabled() const
{
	return m_ssm->getTextureEnabled();
}

osg::PolygonMode::Mode OSGViewerWidget::getPolygonMode() const
{
	return m_ssm->getPolygonMode();
}

void OSGViewerWidget::setLightingEnabled(bool enable)
{
	m_ssm->setLightingEnabled(enable);
	m_thumb_ssm->setLightingEnabled(enable);
}

void OSGViewerWidget::setBackfaceEnabled(bool enable)
{
	m_ssm->setBackfaceEnabled(enable);
	m_thumb_ssm->setBackfaceEnabled(enable);
}

void OSGViewerWidget::setTextureEnabled(bool enable)
{
	m_ssm->setTextureEnabled(enable);
	m_thumb_ssm->setTextureEnabled(enable);
}

void OSGViewerWidget::setPolygonMode(osg::PolygonMode::Mode mode)
{
	m_ssm->setPolygonMode(mode);
	m_thumb_ssm->setPolygonMode(mode);
}

bool OSGViewerWidget::eventFilter(QObject * obj, QEvent * e)
{
	// this is in the key tracker
	//if (e->type() == QEvent::ShortcutOverride) {
	//	if (getActualNavigationMode() == MANIPULATOR_FIRST_PERSON) {
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

QString OSGViewerWidget::debugString()
{
	QString str;

	auto *cam = m_main_view->getCamera();
	double fovy, aspect, n, f;
	cam->getProjectionMatrixAsPerspective(fovy, aspect, n, f);
	str += QString().sprintf("camera near: %f, far %f\n", n, f);

	return str;
}

void OSGViewerWidget::paintEvent(QPaintEvent *e)
{
	// let qt do setup stuff and call paintGL
	QOpenGLWidget::paintEvent(e);
}

void OSGViewerWidget::initializeGL()
{
	recreateFramebuffer();
}

void OSGViewerWidget::paintGL()
{
	QElapsedTimer t; t.start();
	if (!m_viewer) return;
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

	// osg rendering
	if (m_rendering_enabled) {
		QElapsedTimer ft; ft.start();
		m_viewer->frame(dt);
		// we get control back on GraphicsWindowEmbedded::makeCurrentImplementation
		m_frame_time = ft.nsecsElapsed() / 1000000.0;
	}

	m_full_frame_time = t.nsecsElapsed() / 1000000.0;
	m_between_timer.start();

	if (m_fbo) {
		QOpenGLFramebufferObject::blitFramebuffer(nullptr, m_fbo.get(),
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
}

void OSGViewerWidget::resizeGL(int width, int height)
{
	this->getEventQueue()->windowResize(this->x(), this->y(), width, height);
	m_graphics_window->resized(this->x(), this->y(), width, height);
	// ^ this also fixes camera aspect ratio and viewport
	// no need to do it manually

	recreateFramebuffer();

}

void OSGViewerWidget::setThumbnailSize(QSize size)
{
	m_thumb_graphics_window->resized(0, 0, size.width(), size.height());
	m_thumb_size = size;

	recreateFramebuffer();
}

void OSGViewerWidget::keyPressEvent(QKeyEvent* event)
{
	if (m_manipulator == MANIPULATOR_FLIGHT) {
		if (event->key() == Qt::Key_Alt) {
			flightStartStrafe();
		}
	}

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

	// how do we know whether to ignore or not?
	// ex: flight mode slow down causes cancel?
	// ideally we'd send this mouse event over to the manipulator, check for ignore
	event->ignore();


	//if (m_manipulator == MANIPULATOR_OBJECT
	//	|| m_manipulator == MANIPULATOR_SIMPLE) {
	//	event->ignore();
	//}
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
	m_key_tracker->releaseAll();
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

void OSGViewerWidget::centerCursor()
{
	QCursor new_cursor = cursor();
	new_cursor.setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
	setCursor(new_cursor);
}

void OSGViewerWidget::recreateFramebuffer()
{
	if (!context()) return;
	makeCurrent();

	QOpenGLFramebufferObjectFormat fmt;
	fmt.setSamples(m_samples);
	fmt.setAttachment(QOpenGLFramebufferObject::Depth);

	m_fbo = std::make_shared<QOpenGLFramebufferObject>(size(), fmt);
	m_graphics_window->setFramebuffer(m_fbo);

	QOpenGLFramebufferObjectFormat thumb_fmt;
	thumb_fmt.setSamples(0);
	thumb_fmt.setAttachment(QOpenGLFramebufferObject::Depth);

	m_thumb_fbo = std::make_shared<QOpenGLFramebufferObject>(m_thumb_size, thumb_fmt);
	m_thumb_graphics_window->setFramebuffer(m_thumb_fbo);
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
	osgGA::EventQueue* eventQueue = m_graphics_window->getEventQueue();

	if (eventQueue)
		return eventQueue;
	else
		throw std::runtime_error("Unable to obtain valid event queue");
}
