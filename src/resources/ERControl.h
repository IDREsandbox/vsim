#ifndef ERCONTROL_H
#define ERCONTROL_H

#include <QObject>
#include <osg/Node>
#include <QUndoStack>
#include <QSortFilterProxyModel>
#include <vector>
#include <memory>

#include "Core/Command.h"

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
class ERBar;

// manages which ER is active
// slots for creating a new ER, editing, etc
class ERControl : public QObject 
{
	Q_OBJECT;
public:
	ERControl(VSimApp *app, MainWindow *window, QObject *parent = nullptr);

	void load(EResourceGroup *ers);

	// actions
	void newER();
	void deleteER();
	void editERInfo();
	void openTopResource(); // open the file
	void launchResource(const EResource *res);
	void setPosition();
	void mergeERs(const EResourceGroup *ers);

	void gotoPosition();

	void lockResources();
	void unlockResources();

	// -1 to hide
	void setDisplay(EResource *res, bool go = true);
	//void goToResource(EResource *res, bool instant = false);
	void closeER();
	void closeAll();

	void showAll(bool all);
	bool showingAll() const;

	// callbacks, show and goto resource
	void onTouch();
	void onTopChange();
	void onSelectionChange();
	void onRestrictToCurrent();
	void onReset();

	// selection
	void addToSelection(EResource *res, bool top = true);
	void selectERs(const std::vector<EResource*> &res);
	std::set<size_t> getCombinedSelection() const;
	std::vector<EResource*> getCombinedSelectionP() const;
	EResource *getCombinedLastSelectedP() const;

	// only selectable if visible in a visible box
	bool isSelectable(EResource *res) const;
	void clearSelection();

	// filters
	void resetFilters();
	void setRadius(float radius);
	void enableAutoLaunch(bool enable);

	void onUpdate();

	QString debugString();
	void debug();

signals:
	void sRadiusChanged(float radius);

public: // actions
	QAction *a_new_er;
	QAction *a_delete_er;
	QAction *a_edit_er;
	QAction *a_open_er;
	QAction *a_position_er;
	QAction *a_goto_er;
	QAction *a_lock_ers;
	QAction *a_unlock_ers;
	QAction *a_close_er;
	QAction *a_close_all;

private:
	VSimApp *m_app;
	MainWindow *m_window;
	ERBar *m_bar;

	EResourceGroup *m_ers;
	ECategoryGroup *m_categories;

	bool m_going_to;
	EResource *m_displayed;
	ERDisplay *m_display;
	ERFilterArea *m_filter_area;

	ERScrollBox *m_global_box;
	ERScrollBox *m_local_box;
	ERScrollBox *m_all_box;
	ERScrollBox *m_last_touched;

	QUndoStack *m_undo_stack;

	// filter stuff
	std::unique_ptr<ERFilterSortProxy> m_filter_proxy;
	std::unique_ptr<ERFilterSortProxy> m_global_proxy;
	std::unique_ptr<ERFilterSortProxy> m_local_proxy;

	CheckableListProxy *m_category_checkbox_model;

	ECategoryControl *m_category_control;

	float m_radius;
	bool m_enabled;
	bool m_auto_launch;

	double m_update_time_sec;
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

#endif
