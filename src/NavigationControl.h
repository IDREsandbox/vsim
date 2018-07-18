#ifndef NAVIGATIONCONTROL_H
#define NAVIGATIONCONTROL_H

#include <set>
#include <osg/Node>
#include <QWidget>
#include <QAction>
#include <QActionGroup>
#include <osgGA/StateSetManipulator>
#include "Navigation.h"

class VSimApp;
class OSGViewerWidget;

// Bridges osg and gui for narratives
class NavigationControl : public QObject
{
	Q_OBJECT
public:
	NavigationControl(VSimApp *app,
		OSGViewerWidget *viewer,
		QObject *parent = nullptr);

	void activate(); // sets vsim state

	void onModeChange(Navigation::Mode mode);

public: // actions
	QActionGroup *m_navigation_action_group;
	QAction *a_first_person;
	QAction *a_flight;
	QAction *a_object;
	QAction *a_freeze;
	QAction *a_home;
	QAction *a_ground_mode;
	QAction *a_collisions;

	QAction *a_lighting;
	QAction *a_backface_culling;
	QAction *a_texturing;
	QAction *a_stats;

	QActionGroup *m_mode_group;
	QAction *a_cycle_mode;
	QAction *a_fill;
	QAction *a_wireframe;
	QAction *a_point;

	QAction *a_speed_up; // forwarded from osg widget
	QAction *a_slow_down;

private:
	VSimApp *m_app;
	OSGViewerWidget *m_viewer;
	osgGA::StateSetManipulator *m_ssm;

	Navigation::Mode m_mode;
	bool m_frozen;
	bool m_enabled;
};

#endif
