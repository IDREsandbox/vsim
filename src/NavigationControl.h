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
	NavigationControl(VSimApp *app,
		OSGViewerWidget *viewer,
		QMenu *menu,
		QObject *parent = nullptr);

	void initMenu(QMenu *menu);

	void activate(); // sets vsim state

	void onModeChange(Navigation::Mode mode);

public: // actions
	QAction *a_first_person;
	QAction *a_flight;
	QAction *a_object;
	QAction *a_freeze;
	QAction *a_home;
	QAction *a_gravity;
	QAction *a_collisions;

private:
	VSimApp *m_app;
	OSGViewerWidget *m_viewer;

	QActionGroup *m_navigation_action_group;
	QMenu *m_menu;

	Navigation::Mode m_mode;
	bool m_frozen;
	bool m_enabled;
};

#endif
