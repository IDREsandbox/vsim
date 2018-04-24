#include "NavigationControl.h"
#include <QMenu>
#include "VSimApp.h"
#include "OSGViewerWidget.h"

NavigationControl::NavigationControl(VSimApp *app, OSGViewerWidget *viewer, QMenu *menu, QObject *parent)
	: QObject(parent),
	m_app(app),
	m_viewer(viewer),
	m_menu(menu)
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
	initMenu(menu);
	onModeChange(m_viewer->getNavigationMode());

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
		m_viewer->setCameraFrozen(a_freeze->isChecked());
		activate();
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
}

void NavigationControl::initMenu(QMenu * menu)
{
	menu->setTitle("Navigation");
	menu->addSeparator()->setText("Navigation Mode");
	menu->addAction(a_first_person);
	menu->addAction(a_flight);
	menu->addAction(a_object);
	menu->addSeparator();
	menu->addAction(a_freeze);
	menu->addAction(a_home);
	menu->addSeparator();
	menu->addAction(a_gravity);
	menu->addAction(a_collisions);
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
