#include "NavigationControl.h"

#include <QMenu>
#include <QVector3D>
#include <QDebug>

#include "VSimApp.h"
#include "OSGViewerWidget.h"
#include "VSimRoot.h"
#include "Core/TypesSerializer.h"
#include "types_generated.h"
#include "settings_generated.h"
#include "Core/Util.h"
#include "NavigationSettingsDialog.h"
#include "DisplaySettingsDialog.h"
#include "PositionDialog.h"

static QString onoff(bool on) {
	if (on) return "On";
	else return "Off";
}

NavigationControl::NavigationControl(VSimApp *app, OSGViewerWidget *viewer, QObject *parent)
	: QObject(parent),
	m_app(app),
	m_viewer(viewer)
{

	// navigation actions	
	m_navigation_action_group = new QActionGroup(this);
	m_navigation_action_group->setExclusive(true);
	a_first_person = new QAction(m_navigation_action_group);
	a_flight = new QAction(m_navigation_action_group);
	a_object = new QAction(m_navigation_action_group);
	a_first_person->setCheckable(true);
	a_flight->setCheckable(true);
	a_object->setCheckable(true);
	a_first_person->setText("First Person");
	a_flight->setText("Flight");
	a_object->setText("Object");
	a_first_person->setShortcut(Qt::Key_1);
	a_flight->setShortcut(Qt::Key_2);
	a_object->setShortcut(Qt::Key_3);

	a_freeze = new QAction(this);
	a_freeze->setCheckable(true);
	a_freeze->setText("Freeze");
	a_freeze->setShortcut(Qt::Key_Space);

	a_home = new QAction(this);
	a_home->setText("Reset");
	a_home->setShortcut(Qt::Key_R);

	a_ground_mode = new QAction(this);
	a_ground_mode->setCheckable(true);
	a_ground_mode->setText("Ground Mode");
	a_ground_mode->setShortcut(Qt::Key_G);

	a_collisions = new QAction(this);
	a_collisions->setCheckable(true);
	a_collisions->setText("Collisions");
	a_collisions->setShortcut(Qt::Key_C);

	// gui -> osg
	connect(m_navigation_action_group, &QActionGroup::triggered, this,
		[this](QAction *which) {
		if (!m_enabled) return;
		if (which == a_first_person) {
			m_viewer->setNavigationMode(Navigation::FIRST_PERSON);
		}
		else if (which == a_flight) {
			m_viewer->setNavigationMode(Navigation::FLIGHT);
		}
		else if (which == a_object) {
			m_viewer->setNavigationMode(Navigation::OBJECT);
		}
		m_viewer->setCameraFrozen(false);
		a_freeze->setChecked(false);
		activate();
	});
	connect(a_freeze, &QAction::triggered, this,
		[this]() {
		if (!m_enabled) return;
		// unfreeze if frozen
		// freeze if not
		bool frozen = m_viewer->getCameraFrozen();
		m_viewer->setCameraFrozen(!frozen);
		activate();

		QString message = m_viewer->getCameraFrozen() ?
			"Camera Freeze" : "Camera Unfreeze";
		m_app->setStatusMessage(message);
	});
	connect(a_home, &QAction::triggered, this,
		[this]() {
		qInfo() << "Reset Position";
		m_viewer->goHome();
		activate();
		m_app->setStatusMessage("Reset");
	});
	connect(a_ground_mode, &QAction::triggered, this,
		[this]() {
		if (!m_enabled) return;
		bool enable = !m_viewer->groundModeEnabled();
		qInfo() << "Ground mode" << enable;
		m_viewer->enableGroundMode(enable);
		activate();
		QString onoff = enable ? "On" : "Off";
		m_app->setStatusMessage("Ground Mode " + onoff);
	});
	connect(a_collisions, &QAction::triggered, this,
		[this]() {
		if (!m_enabled) return;
		bool enable = !m_viewer->collisionsEnabled();
		qInfo() << "Collisions" << enable;
		m_viewer->enableCollisions(enable);
		activate();
		QString onoff = enable ? "On" : "Off";
		m_app->setStatusMessage("Collisions " + onoff);
	});

	// osg -> gui
	connect(m_viewer, &OSGViewerWidget::sCameraFrozen, a_freeze, &QAction::setChecked);
	connect(m_viewer, &OSGViewerWidget::sNavigationModeChanged, this, &NavigationControl::onModeChange);
	connect(m_viewer, &OSGViewerWidget::sGroundModeChanged, a_ground_mode, &QAction::setChecked);
	connect(m_viewer, &OSGViewerWidget::sCollisionsChanged, a_collisions, &QAction::setChecked);


	a_lighting = new QAction(this);
	a_lighting->setCheckable(true);
	a_lighting->setText("Lighting");
	a_lighting->setShortcut(Qt::Key_L);
	connect(a_lighting, &QAction::triggered, this,
		[this](bool checked) {
		m_viewer->setLightingEnabled(checked);
		m_app->setStatusMessage("Lighting " + onoff(checked));
	});

	a_backface_culling = new QAction(this);
	a_backface_culling->setCheckable(true);
	a_backface_culling->setText("Backface Culling");
	a_backface_culling->setShortcut(Qt::Key_B);
	connect(a_backface_culling, &QAction::triggered, this,
		[this](bool checked) {
		m_viewer->setBackfaceEnabled(checked);
		m_app->setStatusMessage("Backface Culling " + onoff(checked));
	});

	a_texturing = new QAction(this);
	a_texturing->setCheckable(true);
	a_texturing->setText("Texturing");
	a_texturing->setShortcut(Qt::Key_X);
	connect(a_texturing, &QAction::triggered, this,
		[this](bool checked) {
		m_viewer->setTextureEnabled(checked);
		m_app->setStatusMessage("Texturing " + onoff(checked));
	});

	a_stats = new QAction(this);
	a_stats->setText("Stats");
	a_stats->setShortcut(Qt::Key_Y);
	connect(a_stats, &QAction::triggered, this,
		[this](bool checked) {
		m_viewer->cycleStats();
	});

	m_mode_group = new QActionGroup(this);
	m_mode_group->setExclusive(true);
	connect(m_mode_group, &QActionGroup::triggered, this,
		[this](QAction *which) {
		QString text;
		if (which == a_fill) {
			m_viewer->setPolygonMode(osg::PolygonMode::Mode::FILL);
			text = "Fill";
		}
		else if (which == a_wireframe) {
			m_viewer->setPolygonMode(osg::PolygonMode::Mode::LINE);
			text = "Line";
		}
		else {
			m_viewer->setPolygonMode(osg::PolygonMode::Mode::POINT);
			text = "Point";
		}
		m_app->setStatusMessage("Polygon Mode: " + text);
	});

	a_cycle_mode = new QAction(this);
	a_cycle_mode->setText("Change Draw Mode");
	a_cycle_mode->setShortcut(Qt::Key_Semicolon);
	connect(a_cycle_mode, &QAction::triggered, this, [this]() {
		//auto *ssm = m_viewer
		QAction *next = a_fill;
		if (a_fill->isChecked()) {
			next = a_wireframe;
		}
		else if (a_wireframe->isChecked()) {
			next = a_point;
		}
		else {
			next = a_fill;
		}
		next->trigger();
	});

	a_fill = new QAction(this);
	a_fill->setCheckable(true);
	a_fill->setText("Fill");
	a_fill->setShortcut(Qt::Key_U);
	m_mode_group->addAction(a_fill);

	a_wireframe = new QAction(this);
	a_wireframe->setCheckable(true);
	a_wireframe->setText("Wireframe");
	a_wireframe->setShortcut(Qt::Key_I);
	m_mode_group->addAction(a_wireframe);

	a_point = new QAction(this);
	a_point->setCheckable(true);
	a_point->setText("Point Cloud");
	a_point->setShortcut(Qt::Key_O);
	m_mode_group->addAction(a_point);

	// speed tick
	a_speed_up = m_viewer->a_speed_up;
	a_slow_down = m_viewer->a_slow_down;
	connect(m_viewer, &OSGViewerWidget::sSpeedActivelyChanged, this,
		[this](int tick, double multiplier) {
		m_app->setStatusMessage("Speed: " + QString::number(tick)
			+ " (x" + QString::number(multiplier, 'g', 4) + ")");
	});

	a_navigation_settings = new QAction(this);
	a_navigation_settings->setText("Navigation Settings");

	a_display_settings = new QAction(this);
	a_display_settings->setText("Display Settings");

	a_reposition = new QAction(this);
	a_reposition->setText("Set Current Position");

	connect(a_navigation_settings, &QAction::triggered, this, &NavigationControl::execNavigationSettings);
	connect(a_display_settings, &QAction::triggered, this, &NavigationControl::execDisplaySettings);
	connect(a_reposition, &QAction::triggered, this, &NavigationControl::execSetPosition);

	// app -> this
	connect(m_app, &VSimApp::sStateChanged, this,
		[this](VSimApp::State old, VSimApp::State current) {
		if (!m_app->isFlying()) {
			m_viewer->setCameraFrozen(true);
			m_app->stopGoingSomewhere();
		}
		else {
			m_viewer->setFocus();
		}
	});
	connect(m_app, &VSimApp::sAboutToSave, this,
		[this]() {
		gatherSettings();
	});
	connect(m_app, &VSimApp::sReset, this,
		[this]() {
		extractSettings();

		m_viewer->startup();

		// init render stuff
		a_lighting->setChecked(m_viewer->getLightingEnabled());
		a_backface_culling->setChecked(m_viewer->getBackfaceEnabled());
		a_texturing->setChecked(m_viewer->getTextureEnabled());

		// initialize mode
		auto pmode = m_viewer->getPolygonMode();
		if (pmode == osg::PolygonMode::FILL) {
			a_fill->setChecked(true);
		}
		else if (pmode == osg::PolygonMode::LINE) {
			a_wireframe->setChecked(true);
		}
		else {
			a_point->setChecked(true);
		}
		// initialize
		onModeChange(m_viewer->getNavigationMode());
		onLockChange();
	});
	connect(m_app->getRoot(), &VSimRoot::sDisableNavigationChanged, this, &NavigationControl::onLockChange);
}

void NavigationControl::activate()
{
	if (m_app->isPlaying()) {
		m_app->setState(VSimApp::PLAY_FLYING);
	}
	else {
		m_app->setState(VSimApp::EDIT_FLYING);
	}
}

void NavigationControl::gatherSettings()
{
	namespace fbs = VSim::FlatBuffers;

	// navigation settings
	auto *ns = &m_app->getRoot()->navigationSettings();

	auto home = TypesSerializer::osg2fbCameraMatrix(m_viewer->homePosition());
	ns->start_camera = std::make_unique<fbs::CameraPosDirUp>(home);
	ns->default_start_camera = m_viewer->usingDefaultHomePosition();
	ns->length_unit = (VSim::FlatBuffers::LengthUnit)m_viewer->lengthUnit();

	ns->base_speed = m_viewer->baseSpeed();
	ns->flight_speed_tick = m_viewer->startupSpeedTick();

	ns->collisions_on = m_viewer->collisionOnStartup();
	ns->collision_radius = m_viewer->collisionRadius();

	ns->ground_mode_on = m_viewer->groundOnStartup();
	ns->gravity_acceleration = -m_viewer->gravityAcceleration();
	ns->gravity_fall_speed = m_viewer->gravitySpeed();
	ns->eye_height = m_viewer->eyeHeight();

	// graphics settings
	auto *gs = &m_app->getRoot()->graphicsSettings();
	gs->lighting = m_viewer->getLightingEnabled();

	VSim::FlatBuffers::Color *ambs = Util::getOrCreate(gs->ambient).get();
	QColor qamb = Util::vec3ToColor(m_viewer->getAmbient());
	*ambs = VSim::FlatBuffers::Color(TypesSerializer::qt2fbColor(qamb));

	auto *cs = Util::getOrCreate(gs->camera_settings).get();

	// camera setings
	cs->auto_clip = m_viewer->autoClip();
	cs->near_clip = m_viewer->nearClip();
	cs->far_clip = m_viewer->farClip();
	cs->fovy = m_viewer->fovy();
}

void NavigationControl::extractSettings()
{
	namespace fbs = VSim::FlatBuffers;

	// navigation settings
	auto *ns = &m_app->getRoot()->navigationSettings();

	// start camera
	if (ns->default_start_camera) {
		m_viewer->resetHomePosition();
	}
	else {
		auto &start_camera = ns->start_camera;
		if (start_camera) {
			m_viewer->setHomePosition(
				TypesSerializer::fb2osgCameraMatrix(*start_camera));
		}
		else {
			m_viewer->resetHomePosition();
		}
	}
	m_viewer->setLengthUnit((LengthUnit)ns->length_unit);

	m_viewer->setBaseSpeed(ns->base_speed);
	m_viewer->setStartupSpeedTick(ns->flight_speed_tick);

	m_viewer->setCollisionOnStartup(ns->collisions_on);
	m_viewer->enableCollisions(ns->collisions_on);
	m_viewer->setCollisionRadius(ns->collision_radius);

	m_viewer->setGroundOnStartup(ns->ground_mode_on);
	m_viewer->enableGroundMode(ns->ground_mode_on);
	m_viewer->setGravityAcceleration(-ns->gravity_acceleration);
	m_viewer->setGravitySpeed(ns->gravity_fall_speed);

	m_viewer->setEyeHeight(ns->eye_height);


	// graphics settings
	auto *gs = &m_app->getRoot()->graphicsSettings();
	m_viewer->setLightingEnabled(gs->lighting);

	auto *ambient = gs->ambient.get();
	osg::Vec3 amb;
	if (ambient) {
		amb = Util::colorToVec3(TypesSerializer::fb2qtColor(*ambient));
	}
	else {
		amb = OSGViewerWidget::defaultAmbient();
	}
	m_viewer->setAmbient(amb);

	// camera settings
	auto &cs = Util::getOrCreate(gs->camera_settings);

	m_viewer->setFovy(cs->fovy);
	m_viewer->setNearClip(cs->near_clip);
	m_viewer->setFarClip(cs->far_clip);
	m_viewer->setAutoClip(cs->auto_clip);
}

void NavigationControl::onModeChange(Navigation::Mode mode)
{
	if (mode == Navigation::FIRST_PERSON) {
		a_first_person->setChecked(true);
	}
	else if (mode == Navigation::FLIGHT) {
		a_flight->setChecked(true);
	}
	else if (mode == Navigation::OBJECT) {
		a_object->setChecked(true);
	}
	m_app->setStatusMessage(
		QString(Navigation::ModeStrings[mode]) + " Mode");
}

void NavigationControl::onLockChange()
{
	bool locked = m_app->getRoot()->navigationLocked();
	
	m_enabled = !locked;
	bool enable = !locked;

	a_first_person->setEnabled(enable);
	a_flight->setEnabled(enable);
	a_object->setEnabled(enable);
	a_freeze->setEnabled(enable);
	a_home->setEnabled(enable);
	a_ground_mode->setEnabled(enable);
	a_collisions->setEnabled(enable);

	a_reposition->setEnabled(enable);

	m_viewer->setCameraFrozen(true);
}

void NavigationControl::execNavigationSettings()
{
	NavigationSettingsDialog dlg(m_app);
	dlg.exec();
}

void NavigationControl::execDisplaySettings()
{
	DisplaySettingsDialog dlg(m_app);
	dlg.exec();
}

void NavigationControl::execSetPosition()
{
	qDebug() << "exec set position";
	if (m_app->getRoot()->settingsLocked()) return;
	PositionDialog dlg;

	auto mat = m_app->getCameraMatrix();
	auto p0 = mat.getTrans();
	double y, p, r;
	Util::matToYPR(mat, &y, &p, &r);

	dlg.setPosition(QVector3D(p0.x(), p0.y(), p0.z()));
	dlg.setOrientationDeg(QVector3D(Util::deg(y), Util::deg(p), Util::deg(r)));

	int result = dlg.exec();
	if (result == QDialog::Rejected) return;

	QVector3D p1 = dlg.position();
	QVector3D o1 = dlg.orientationDeg();

	auto translate = osg::Matrixd::translate(p1.x(), p1.y(), p1.z());
	auto rotate = Util::yprToMat(Util::rad(o1.x()), Util::rad(o1.y()), Util::rad(o1.z()));

	m_viewer->setCameraMatrix(rotate * translate);
}

