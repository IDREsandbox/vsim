#ifndef GROUPSCROLLBOX_H
#define GROUPSCROLLBOX_H

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QDebug>
#include <QtWidgets/QMenu>
#include <QContextMenuEvent>
#include <QDrag>
#include <set>
#include <QTimer>

#include "Group.h"
#include "HorizontalScrollBox.h"

// Connects a Group to a scroll box
// Children classes override createItem to create a specific item
// based on the group type
class GroupScrollBox : public HorizontalScrollBox {
	Q_OBJECT
public:
	GroupScrollBox(QWidget* parent);

	void clear();
	void reload();

	// Data tracking
	virtual void setGroup(Group *group);
	Group *getGroup() const;

	// 
	virtual ScrollBoxItem* createItem(osg::Node*);

signals:
	void sSelectionChange();
	// void sMove(std::set<int> from, int to); // move is in HorizontalScrollBox
	void sDelete();
	void sNew();

	// void sSelectionCleared(); // in HorizontalScrollBox

protected:
	Group *m_group;

};

#endif