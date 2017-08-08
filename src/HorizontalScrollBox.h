#ifndef HORIZONTALSCROLLBOX_H
#define HORIZONTALSCROLLBOX_H

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QDebug>
#include <QtWidgets/QMenu>
#include <QContextMenuEvent>
#include <set>

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

	ScrollBoxItem *getItem(int position);
	void clear();

	// selection
	void deleteSelection();
	void clearSelection();
	const std::set<int>& getSelection();
	int getLastSelected();
	//void select(ScrollBoxItem*);
	void addToSelection(int);
	void removeFromSelection(int);
	void select(int);
	bool isSelected(int);
	void forceSelect(int); // hack, for the narrative player, does not emit signals

	void setSpacing(int);

	void setMenu(QMenu *menu);
	void setItemMenu(QMenu *menu);

	// Data tracking
	void setGroup(Group *group);

signals:
	void sSelectionChange();

protected:
	// these are used to link new/delete signals from groups to creation of new items
	void insertNewItem(int position);
	virtual ScrollBoxItem *createItem(osg::Node*);

	// qt overrides
	virtual void resizeEvent(QResizeEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

	void mousePressEvent(QMouseEvent* event);
	void itemMousePressEvent(QMouseEvent* event, int index);

protected:
	void refresh(); // redraw items (after deletion or something)
	//void setWidgetWidth();
	//void positionChildren();
	//void refreshGeometry();
	
	QWidget* m_scroll_area_widget;
	QList<ScrollBoxItem*> m_items;

	int m_last_selected;
	std::set<int> m_selection;

	//int boxHeight() const;
	//int boxWidth() const;
	int m_height;
	int m_spacing = 10;
	//float m_ratio; // width to height ratio for items
	//float m_space_ratio; // TODO: width to height ratio for spaces

	QMenu *m_menu;
	QMenu *m_item_menu;

	Group *m_group;

};

#endif // HORIZONTALSCROLLBOX_H