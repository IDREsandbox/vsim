#ifndef ERFILTERCONNECTOR_H
#define ERFILTERCONNECTOR_H

#include <QObject>

class VSimApp;
class MainWindow;
class EResource;
class EResourceGroup;
class ECategoryGroup;
class ERDialog;
class ERDisplay;
class ERScrollBox;
class ECategoryControl;
class ERFilterSortProxy;
class ERFilterArea;
class CheckableListProxy;
class SelectionStack;

// manages which ER is active
// slots for creating a new ER, editing, etc
class ERFilterConnector : public QObject 
{
	Q_OBJECT;
public:
	ERFilterConnector(ERControl *control, MainWindow *window, QObject *parent = nullptr);
	ERControl(VSimApp *app, MainWindow *window, QObject *parent = nullptr);


private:
	VSimApp *m_app;
	MainWindow *m_window;

	EResourceGroup *m_ers;
	ECategoryGroup *m_categories;

	int m_active_item;
	ERDisplay *m_display;
	ERFilterArea *m_filter_area;

	ERScrollBox *m_global_box;
	ERScrollBox *m_local_box;
	SelectionStack *m_global_selection;
	SelectionStack *m_local_selection;

	QUndoStack *m_undo_stack;

	// filter stuff
	std::unique_ptr<ERFilterSortProxy> m_filter_proxy;
	std::unique_ptr<ERFilterSortProxy> m_global_proxy;
	std::unique_ptr<ERFilterSortProxy> m_local_proxy;

	CheckableListProxy *m_category_checkbox_model;

	ECategoryControl *m_category_control;

	//osg::Vec3 m_prev_position;
	//bool m_pos_dirty;
};

#endif
