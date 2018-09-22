#ifndef ERCONTROL_H
#define ERCONTROL_H

#include <QObject>
#include <osg/Node>
#include <QUndoStack>
#include <QSortFilterProxyModel>
#include <vector>
#include <memory>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

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
class ECategoryLegend;
namespace VSim { namespace FlatBuffers { struct ERSettingsT; } };

// manages which ER is active
// slots for creating a new ER, editing, etc
class ERControl : public QObject 
{
	Q_OBJECT;
public:
	ERControl(VSimApp *app, MainWindow *window, QObject *parent = nullptr);
	~ERControl();

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
	void setDisplay(EResource *res, bool go = true, bool fade = true);
	//void goToResource(EResource *res, bool instant = false);
	void closeER();
	void closeAll();

	void showAll(bool all);
	bool showingAll() const;

	bool isERState() const;

	// callbacks, show and goto resource
	void onTouch();
	void onTopChange();
	void onSelectionChange();
	void onRestrictToCurrent();
	void onReset();
	void onYearsEnabledChanged();

	void showLegend(bool show);

	void gatherSettings();
	void extractSettings();
	void loadFilterSettings(VSim::FlatBuffers::ERSettingsT &settings);

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
	void enableYears(bool enable);

	void onUpdate();

	QString debugString();
	void debug();

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
	void checkShowLegend();

private:
	VSimApp *m_app;
	QUndoStack *m_undo_stack;
	EResourceGroup *m_ers;
	ECategoryGroup *m_categories;

	MainWindow *m_window;
	ERBar *m_bar;
	EResource *m_displayed;
	ERDisplay *m_display;
	ERFilterArea *m_filter_area;
	ERScrollBox *m_global_box;
	ERScrollBox *m_local_box;
	ERScrollBox *m_all_box;
	ERScrollBox *m_last_touched;
	ECategoryLegend *m_legend;
	QGraphicsOpacityEffect *m_legend_opacity;
	QPropertyAnimation *m_legend_out_anim;
	QPropertyAnimation *m_legend_in_anim;

	// filter stuff
	std::unique_ptr<ERFilterSortProxy> m_filter_proxy;
	std::unique_ptr<ERFilterSortProxy> m_all_proxy;
	std::unique_ptr<ERFilterSortProxy> m_global_proxy;
	std::unique_ptr<ERFilterSortProxy> m_local_proxy;

	CheckableListProxy *m_category_checkbox_model;

	ECategoryControl *m_category_control;

	bool m_years_enabled;
	bool m_showing_all;
	bool m_going_to;
	float m_radius;
	bool m_enabled;
	bool m_auto_launch;
	bool m_show_legend;

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
