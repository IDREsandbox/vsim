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
#include "GroupTemplate.h"
#include "HorizontalScrollBox.h"

// Connects a Group to a scroll box
// Children classes override createItem to create a specific item
// based on the group type
template <class T>
class GroupScrollBox : public HorizontalScrollBox {
public:
	GroupScrollBox(QWidget* parent);

	// Data tracking
	virtual void setGroup(TGroup<T> *group);
	TGroup<T> *getGroup() const;

	void reload();

protected:
	//virtual ScrollBoxItem* createItem(size_t index);
	virtual ScrollBoxItem* createItem(T *node);

protected:
	TGroup<T> *m_group;
};

#include "GroupScrollBox.inl"
#endif