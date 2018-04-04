#ifndef ERCONTROL_H
#define ERCONTROL_H

#include <QObject>
#include <osg/Node>
#include <QUndoStack>
#include <QSortFilterProxyModel>
#include <vector>

#include "Command.h"

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
class ERControl : public QObject 
{
	Q_OBJECT;
public:
	ERControl(VSimApp *app, MainWindow *window, QObject *parent = nullptr);

	void load(EResourceGroup *ers);

	void newER();
	void deleteER();
	void editERInfo();
	void openResource(); // open the file
	void setPosition();
	void gotoPosition();

	// -1 to hide
	void setDisplay(int index, bool go = true);

	// show and goto resource
	void onSelectionChange();

	void selectERs(const std::vector<EResource*> &res);

	void debug();

public: // actions
	QAction *a_new_er;
	QAction *a_delete_er;
	QAction *a_edit_er;
	QAction *a_open_er;
	QAction *a_position_er;
	QAction *a_goto_er;

private:
	std::set<int> getCombinedSelection();
	std::vector<EResource*> getCombinedSelectionP();
	int getCombinedLastSelected();
	void clearSelection();

private:
	VSimApp *m_app;
	MainWindow *m_window;

	osg::ref_ptr<EResourceGroup> m_ers;
	osg::ref_ptr<ECategoryGroup> m_categories;

	int m_active_item;
	ERDisplay *m_display;
	ERFilterArea *m_filter_area;

	ERScrollBox *m_global_box;
	ERScrollBox *m_local_box;
	SelectionStack *m_global_selection;
	SelectionStack *m_local_selection;

	QUndoStack *m_undo_stack;

	// filter stuff
	osg::ref_ptr<ERFilterSortProxy> m_filter_proxy;
	osg::ref_ptr<ERFilterSortProxy> m_global_proxy;
	osg::ref_ptr<ERFilterSortProxy> m_local_proxy;

	CheckableListProxy *m_category_checkbox_model;

	ECategoryControl *m_category_control;
};

class SelectERCommand : public QUndoCommand {
public:
	SelectERCommand(ERControl *control,
		const std::vector<EResource*> &resources,
		Command::When when = Command::ON_BOTH,
		QUndoCommand *parent = nullptr);
	void undo() override;
	void redo() override;
private:
	ERControl *m_control;
	std::vector<EResource*> m_resources;
	Command::When m_when;
};

#endif /* ERCONTROL_H */
