#ifndef ERCONTROL_H
#define ERCONTROL_H

#include <QObject>
#include <osg/Node>
#include <QUndoStack>
#include <QSortFilterProxyModel>

class EResourceGroup;
class MainWindow;
class ECategoryGroup;
class ERDialog;
class ERDisplay;
class ERScrollBox;
class ECategoryControl;
class ERFilterSortProxy;
class ERFilterArea;
class CheckableListProxy;

// manages which ER is active
// slots for creating a new ER, editing, etc
class ERControl : public QObject 
{
	Q_OBJECT;
public:
	ERControl(QObject *parent, MainWindow *window, EResourceGroup *ers);

	void load(EResourceGroup *ers);

	void newER();
	void deleteER();
	void editERInfo();
	void openResource();
	void setPosition();
	void gotoPosition();

	void debug();

private:
	std::set<int> getCombinedSelection();
	int getCombinedLastSelected();

private:
	MainWindow *m_window;

	osg::ref_ptr<EResourceGroup> m_ers;
	osg::ref_ptr<ECategoryGroup> m_categories;

	ERDisplay *m_display;
	ERFilterArea *m_filter_area;

	ERScrollBox *m_global_box;
	ERScrollBox *m_local_box;

	QUndoStack *m_undo_stack;

	// filter stuff
	osg::ref_ptr<ERFilterSortProxy> m_filter_proxy;
	osg::ref_ptr<ERFilterSortProxy> m_global_proxy;
	osg::ref_ptr<ERFilterSortProxy> m_local_proxy;

	CheckableListProxy *m_category_checkbox_model;

	ECategoryControl *m_category_control;

};


#endif /* ERCONTROL_H */
