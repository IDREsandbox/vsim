#include "NavigationControl.h"
#include <QMenu>
#include "VSimApp.h"
#include "OSGViewerWidget.h"
#include <QDebug>

NavigationControl::NavigationControl(VSimApp *app, OSGViewerWidget *viewer, QObject *parent)
	: QObject(parent),
	m_app(app),
	m_viewer(viewer)
{
	m_ssm = m_viewer->getStateSetManipulator();

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
	a_home->setShortcut(Qt::Key_H);

	a_gravity = new QAction(this);
	a_gravity->setCheckable(true);
	a_gravity->setText("Gravity");
	a_gravity->setShortcut(Qt::Key_G);

	a_collisions = new QAction(this);
	a_collisions->setCheckable(true);
	a_collisions->setText("Collisions");
	a_collisions->setShortcut(Qt::Key_C);

	// initialize
	onModeChange(m_viewer->getNavigationMode());

	// gui -> osg
	connect(m_navigation_action_group, &QActionGroup::triggered, this,
		[this](QAction *which) {
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

		// if changing state then just activate
		if (!m_app->isFlying()) {
			activate();
			a_freeze->setChecked(true);
			m_viewer->setCameraFrozen(true);
		}
		else {
			// if already frozen then unfreeze
			m_viewer->setCameraFrozen(a_freeze->isChecked());
			activate();
		}
	});
	connect(a_home, &QAction::triggered, this,
		[this]() {
		m_viewer->reset();
		activate();
	});
	connect(a_gravity, &QAction::triggered, this,
		[this]() {
		m_viewer->enableGravity(a_gravity->isChecked());
		activate();
	});
	connect(a_collisions, &QAction::triggered, this,
		[this]() {
		m_viewer->enableCollisions(a_collisions->isChecked());
		activate();
	});

	// osg -> gui
	connect(m_viewer, &OSGViewerWidget::sCameraFrozen, a_freeze, &QAction::setChecked);
	connect(m_viewer, &OSGViewerWidget::sNavigationModeChanged, this, &NavigationControl::onModeChange);

	a_lighting = new QAction(this);
	a_lighting->setCheckable(true);
	a_lighting->setText("Lighting");
	a_lighting->setShortcut(Qt::Key_L);
	connect(a_lighting, &QAction::triggered, this,
		[this](bool checked) {
		m_ssm->setLightingEnabled(checked);
	});

	a_backface_culling = new QAction(this);
	a_backface_culling->setCheckable(true);
	a_backface_culling->setText("Backface Culling");
	a_backface_culling->setShortcut(Qt::Key_B);
	connect(a_backface_culling, &QAction::triggered, this,
		[this](bool checked) {
		m_ssm->setBackfaceEnabled(checked);
	});

	a_texturing = new QAction(this);
	a_texturing->setCheckable(true);
	a_texturing->setText("Texturing");
	a_texturing->setShortcut(Qt::Key_X);
	connect(a_texturing, &QAction::triggered, this,
		[this](bool checked) {
		m_ssm->setTextureEnabled(checked);
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
		if (which == a_fill) {
			m_ssm->setPolygonMode(osg::PolygonMode::Mode::FILL);
		}
		else if (which == a_wireframe) {
			m_ssm->setPolygonMode(osg::PolygonMode::Mode::LINE);
		}
		else {
			m_ssm->setPolygonMode(osg::PolygonMode::Mode::POINT);
		}
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
	connect(m_app, &VSimApp::sReset, this,
		[this]() {
		m_viewer->setNavigationMode(Navigation::OBJECT);

		// init render stuff
		a_lighting->setChecked(m_ssm->getLightingEnabled());
		a_backface_culling->setChecked(m_ssm->getBackfaceEnabled());
		a_texturing->setChecked(m_ssm->getTextureEnabled());

		// initialize mode
		auto pmode = m_ssm->getPolygonMode();
		if (pmode == osg::PolygonMode::FILL) {
			a_fill->setChecked(true);
		}
		else if (pmode == osg::PolygonMode::LINE) {
			a_wireframe->setChecked(true);
		}
		else {
			a_point->setChecked(true);
		}
	});
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
}
