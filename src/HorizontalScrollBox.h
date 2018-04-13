#ifndef HORIZONTALSCROLLBOX_H
#define HORIZONTALSCROLLBOX_H

#include <QGridLayout>
#include <QScrollArea>
#include <QtWidgets/QMenu>
#include <QContextMenuEvent>
#include <QDrag>
#include <QTimer>
#include <vector>
#include <set>

class Selection;
class SelectionStack;
class ScrollBoxItem;

//#include "narrative/NarrativeScrollItem.h"

// goals:
// 1. add, insert, delete, select, clear (signals)
// 2. context menu for ^
// 3. copy paste, multiselect, multidelete, drag n drop, other fancy stuff

// configuration
// size, item width/height

class HSBScrollArea : public QWidget {
	Q_OBJECT
public:
	HSBScrollArea(QWidget *parent = nullptr);

protected:
	void paintEvent(QPaintEvent *e) override;
};

class HorizontalScrollBox : public QWidget {
	Q_OBJECT
public:
	HorizontalScrollBox(QWidget* parent);

	// Items
	// this scroll box takes ownership, so just construct with nullptr
	//void addItem(ScrollBoxItem*);
	//void insertItem(int position, ScrollBoxItem*);
	void insertItems(const std::vector<std::pair<size_t, ScrollBoxItem*>>& insertions);
	//void removeItem(int position);
	void removeItems(const std::vector<size_t> indices, bool delete_items = true);
	void moveItems(const std::vector<std::pair<size_t, size_t>> &mapping);

	ScrollBoxItem *getItem(int position);
	
	void clear();

	// selection - these set selection, lastSelected, and emit events
	//void setSelection(const std::set<int>& set, int last);
	//void setLastSelected(int last);
	//int getLastSelected();
	//const std::set<int>& getSelection();
	//Selection *selection() const;
	SelectionStack *selectionStack() const;

	// positioning
	void setSpacing(int);

	// returns the item index under the cursor where point is based on the scroll area widget
	// the float is the position from [-x, 1] where 0 is the left side, 1 is the right side, - is in the space before the widget
	std::pair<int, float> posToIndex(int px);

	void refresh(); // redraw items (after deletion or something)

	// menus
	void setMenu(QMenu *menu);
	void setItemMenu(QMenu *menu);

	// dragging
	void enableDragging(bool enable);
	void setMIMEType(const std::string &type);

	// viewport resize event filter
	bool eventFilter(QObject *o, QEvent *e);

	QScrollArea *scrollArea();

signals:
	//void sSelectionChange();
	//void sLastSelectedChange();
	void sMove(const std::vector<std::pair<size_t, size_t>> &);

	// the selection has actively changed (clicked on something)
	void sTouch();

	// hack for joining these boxes together
	void sSelectionCleared();

protected:
	// qt overrides
	//virtual void wheelEvent(QWheelEvent* event);

	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void itemMousePressEvent(QMouseEvent *event, int index);
	virtual void itemMouseReleaseEvent(QMouseEvent *event, int index);
	virtual void itemMouseDoubleClickEvent(QMouseEvent *event, int index);

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

signals:
	void sSelectionChange();

protected:
	// The auto-sliding effect when you're dragging a widget needs a loop
	void moveTimer();
	//void setWidgetWidth();
	//void positionChildren();
	//void refreshGeometry();

	QGridLayout *m_layout;
	QScrollArea *m_scroll;
	QWidget* m_scroll_area_widget;
	std::vector<ScrollBoxItem*> m_items;
	QWidget *m_drop_highlight;

	Selection *m_selection;
	SelectionStack *m_selection_stack;

	//int boxHeight() const;
	//int boxWidth() const;
	int m_height;
	int m_spacing;
	//float m_ratio; // width to height ratio for items
	//float m_space_ratio; // TODO: width to height ratio for spaces

	// drag and drop has a lot of state
	bool m_mouse_down; // has there been a mouse press?
	QPoint m_mouse_down_pos; // press position
	float m_minimum_drag_dist; // minimum mouse move before drag starts

	QDrag *m_drag; // bool m_dragging; // is dragging?
	bool m_dragging_enabled;
	QPoint m_dragpos; // last known drag point
	int m_drag_index; // last known spacer index for dropping
	int m_drag_press_index; // last pressed item

	QTimer *m_move_timer;

	QMenu *m_menu;
	QMenu *m_item_menu;

	QString m_mime_type;

};

#endif // HORIZONTALSCROLLBOX_H