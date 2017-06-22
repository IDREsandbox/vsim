#ifndef HORIZONTALSCROLLBOX_H
#define HORIZONTALSCROLLBOX_H

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QDebug>
#include <QtWidgets/QMenu>
#include <QContextMenuEvent>
#include <set>

#include "ScrollBoxItem.h"
#include "narrative/NarrativeScrollItem.h"

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
	void insertItem(int position, ScrollBoxItem*);
	void clear();

	void deleteItem(int position);
	void deleteSelection();

	void clearSelection();

	const std::set<int>& getSelection();
	int getLastSelected();
	ScrollBoxItem *getItem(int position);

	// selection
	//void select(ScrollBoxItem*);
	void addToSelection(int);
	void removeFromSelection(int);
	void select(int);

	bool isSelected(int);

	//void slideContextMenu(QContextMenuEvent*);
	
	// internal slots
	// pos: global click position, slide: item index
	void openMenu(QPoint globalPos);
	//void onSpaceMenu(int position);

// public signaling, actions, etc
	QMenu* m_slide_menu; // context menu
	QAction* m_action_new;
	QAction* m_action_delete;
signals:
	void sDoubleClick();
	void sNew(int newIndex);
	void sDeleted(std::set<int>);
	//void sDeleted(int index);

	//void deleted(int);
	//void deleted(list);

protected:
	// qt overrides
	virtual void resizeEvent(QResizeEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

	void mousePressEvent(QMouseEvent* event);

	void itemMousePressEvent(QMouseEvent* event, int index);

private:
	// use this to 
	void refresh(); // redraw items (after deletion or something)
	void setWidgetWidth();
	int getIndexOf(ScrollBoxItem*);
	void positionChildren();
	//void renumber(); // renumber all items
	
	QWidget* m_scroll_area_widget;
	QList<ScrollBoxItem*> m_items;

	//ScrollBoxItem* m_selection; // TODO: range selection
	//int m_selection;

	int m_focus;
	int m_last_selected;
	std::set<int> m_selection;

	//int boxHeight() const;
	//int boxWidth() const;
	int m_height;
	int m_spacing;
	float m_ratio; // width to height ratio for items
	float m_space_ratio; // TODO: width to height ratio for spaces

	
};

#endif // HORIZONTALSCROLLBOX_H