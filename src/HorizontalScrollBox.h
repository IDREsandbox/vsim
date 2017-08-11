#ifndef HORIZONTALSCROLLBOX_H
#define HORIZONTALSCROLLBOX_H

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QDebug>
#include <QtWidgets/QMenu>
#include <QContextMenuEvent>
#include <QDrag>
#include <set>
#include <QTimer>

#include "ScrollBoxItem.h"
#include "Group.h"
//#include "narrative/NarrativeScrollItem.h"

// goals:
// 1. add, insert, delete, select, clear (signals)
// 2. context menu for ^
// 3. copy paste, multiselect, multidelete, drag n drop, other fancy stuff

// configuration
// size, item width/height

class HorizontalScrollBox : public QScrollArea {
	Q_OBJECT
public:
	HorizontalScrollBox(QWidget* parent);

// public interface, controller should wire signals back to these
	// Items

	// this scroll box takes ownership, so just construct with nullptr
	void addItem(ScrollBoxItem*);
	virtual void insertItem(int position, ScrollBoxItem*);
	void deleteItem(int position);

	// these are used to link new/delete signals from groups to creation of new items
	void addNewItem();
	void insertNewItem(uint position);
	virtual ScrollBoxItem *createItem(osg::Node*);
	void moveItems(std::vector<std::pair<int,int>> mapping); // assumes sorted
	ScrollBoxItem *getItem(int position);
	void clear();

	// selection
	void deleteSelection();
	void clearSelection();
	void setSelection(std::set<int>); // does negative checking
	const std::set<int>& getSelection();
	int getLastSelected();
	//void select(ScrollBoxItem*);
	void setLastSelected(int);

	void addToSelection(int);
	void removeFromSelection(int);
	void select(int);
	bool isSelected(int);
	void forceSelect(int); // hack, for the narrative player, does not emit signals

	// positioning
	void setSpacing(int);
	// returns the item index under the cursor where point is based on the scroll area widget
	// the float is the position from [-x, 1] where 0 is the left side, 1 is the right side, - is in the space before the widget
	std::pair<int, float> posToIndex(int px);
	void refresh(); // redraw items (after deletion or something)

	// menus
	void setMenu(QMenu *menu);
	void setItemMenu(QMenu *menu);

	// Data tracking
	void setGroup(Group *group);

signals:
	void sSelectionChange();
	// to is the new index of the first item if the drop were to occur
	void sMove(std::set<int> from, int to);

protected:
	// qt overrides
	virtual void resizeEvent(QResizeEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void itemMousePressEvent(QMouseEvent *event, int index);
	void itemMouseReleaseEvent(QMouseEvent *event, int index);

	void dragEnterEvent(QDragEnterEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

protected:
	// The auto-sliding effect when you're dragging a widget needs a loop
	void moveTimer();
	//void setWidgetWidth();
	//void positionChildren();
	//void refreshGeometry();

	QWidget* m_scroll_area_widget;
	QList<ScrollBoxItem*> m_items;
	QWidget *m_drop_highlight;

	int m_last_selected;
	std::set<int> m_selection;

	//int boxHeight() const;
	//int boxWidth() const;
	int m_height;
	int m_spacing = 10;
	//float m_ratio; // width to height ratio for items
	//float m_space_ratio; // TODO: width to height ratio for spaces

	// drag and drop has a lot of state
	bool m_mouse_down; // has there been a mouse press?
	QPoint m_mouse_down_pos; // press position
	float m_minimum_drag_dist; // minimum mouse move before drag starts

	bool m_dragging; // is dragging?
	QPoint m_dragpos; // last known drag point
	int m_drag_index; // last known spacer index for dropping

	QTimer *m_move_timer;

	QMenu *m_menu;
	QMenu *m_item_menu;

	Group *m_group;

};

#endif // HORIZONTALSCROLLBOX_H