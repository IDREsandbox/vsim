#ifndef NAVIGATIONCONTROL_H
#define NAVIGATIONCONTROL_H

#include <set>
#include <osg/Node>
#include <QWidget>
#include <QAction>
#include <QActionGroup>
#include "Navigation.h"

class VSimApp;
class OSGViewerWidget;

// Bridges osg and gui for narratives
class NavigationControl : public QObject
{
	Q_OBJECT
public:
	NavigationControl(VSimApp *app, OSGViewerWidget *viewer, QObject *parent = nullptr);

	void initMenu(QMenu *menu);

	void activate(); // sets vsim state

	void onModeChange(Navigation::Mode mode);

private:
	VSimApp *m_app;
	OSGViewerWidget *m_viewer;

	QMenu *m_menu;

	QActionGroup *m_navigation_action_group;
	QAction *m_action_first_person;
	QAction *m_action_flight;
	QAction *m_action_object;
	QAction *m_action_freeze;
	QAction *m_action_home;
	QAction *m_action_gravity;
	QAction *m_action_collisions;

	Navigation::Mode m_mode;
	bool m_frozen;
	bool m_enabled;
};

#endif
