#include "NavigationControl.h"
#include <QMenu>
#include "VSimApp.h"
#include "OSGViewerWidget.h"

NavigationControl::NavigationControl(VSimApp *app, OSGViewerWidget *viewer, QObject *parent)
	: QObject(parent),
	m_app(app),
	m_viewer(viewer)
{
	// navigation actions	
	m_navigation_action_group = new QActionGroup(this);
	m_navigation_action_group->setExclusive(true);
	m_action_first_person = new QAction(m_navigation_action_group);
	m_action_flight = new QAction(m_navigation_action_group);
	m_action_object = new QAction(m_navigation_action_group);
	m_action_first_person->setCheckable(true);
	m_action_flight->setCheckable(true);
	m_action_object->setCheckable(true);
	m_action_first_person->setText("First Person");
	m_action_flight->setText("Flight");
	m_action_object->setText("Object");
	m_action_first_person->setShortcut(Qt::Key_1);
	m_action_flight->setShortcut(Qt::Key_2);
	m_action_object->setShortcut(Qt::Key_3);

	m_action_freeze = new QAction(this);
	m_action_freeze->setCheckable(true);
	m_action_freeze->setText("Freeze");
	m_action_freeze->setShortcut(Qt::Key_Space);

	m_action_home = new QAction(this);
	m_action_home->setText("Reset");
	m_action_home->setShortcut(Qt::Key_H);

	m_action_gravity = new QAction(this);
	m_action_gravity->setCheckable(true);
	m_action_gravity->setText("Gravity");
	m_action_gravity->setShortcut(Qt::Key_G);

	m_action_collisions = new QAction(this);
	m_action_collisions->setCheckable(true);
	m_action_collisions->setText("Collisions");
	m_action_collisions->setShortcut(Qt::Key_C);

	// initialize
	onModeChange(m_viewer->getNavigationMode());

	connect(m_navigation_action_group, &QActionGroup::triggered, this,
		[this](QAction *which) {
		if (which == m_action_first_person) {
			m_viewer->setNavigationMode(Navigation::FIRST_PERSON);
		}
		else if (which == m_action_flight) {
			m_viewer->setNavigationMode(Navigation::FLIGHT);
		}
		else if (which == m_action_object) {
			m_viewer->setNavigationMode(Navigation::OBJECT);
		}
		m_viewer->setCameraFrozen(false);
		m_action_freeze->setChecked(false);
	});
	connect(m_action_freeze, &QAction::triggered, this,
		[this]() {
		m_viewer->setCameraFrozen(m_action_freeze->isChecked());
	});
	connect(m_action_home, &QAction::triggered, this,
		[this]() {
		m_viewer->reset();
	});

	connect(m_action_gravity, &QAction::toggled, m_viewer, &OSGViewerWidget::enableGravity);
	connect(m_action_collisions, &QAction::toggled, m_viewer, &OSGViewerWidget::enableCollisions);

	// osg -> gui
	connect(m_viewer, &OSGViewerWidget::sCameraFrozen, m_action_freeze, &QAction::setChecked);
	connect(m_viewer, &OSGViewerWidget::sNavigationModeChanged, this, &NavigationControl::onModeChange);
}

void NavigationControl::initMenu(QMenu * menu)
{
	m_menu = menu;
	m_menu->setTitle("Navigation");
	m_menu->addSeparator()->setText("Navigation Mode");
	m_menu->addAction(m_action_first_person);
	m_menu->addAction(m_action_flight);
	m_menu->addAction(m_action_object);
	m_menu->addSeparator();
	m_menu->addAction(m_action_freeze);
	m_menu->addAction(m_action_home);
	m_menu->addSeparator();
	m_menu->addAction(m_action_gravity);
	m_menu->addAction(m_action_collisions);
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
		m_action_first_person->setChecked(true);
	}
	else if (mode == Navigation::FLIGHT) {
		m_action_flight->setChecked(true);
	}
	else if (mode == Navigation::OBJECT) {
		m_action_object->setChecked(true);
	}
}
