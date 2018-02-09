#include <QScrollBar>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <vector>
#include <algorithm>

#include "GroupScrollBox.h"

GroupScrollBox::GroupScrollBox(QWidget* parent)
	: HorizontalScrollBox(parent)
{
}

void GroupScrollBox::reload()
{
	clear();
	if (!m_group) return;

	// create items
	if (m_group) {
		std::vector<std::pair<size_t, ScrollBoxItem*>> items;
		for (uint i = 0; i < m_group->getNumChildren(); i++) {
			ScrollBoxItem *item = createItem(m_group->child(i));
			items.push_back({ i, item });
		}
		insertItems(items);
	}

	refresh();
}

void GroupScrollBox::setGroup(Group * group)
{
	// disconnect incoming signals if already connected to a narrative
	if (m_group != nullptr) disconnect(m_group, 0, this, 0);

	m_group = group;

	// listen to the insert and remove signals from group
	// so that we can add/remove from the slide box accordingly
	// replace with set operations
	//connect(m_group, &Group::sNew, this, &HorizontalScrollBox::insertNewItem);
	//connect(m_group, &Group::sDelete, this, &HorizontalScrollBox::deleteItem);
	//connect(m_group, &Group::sItemsMoved, this, &HorizontalScrollBox::moveItems);

	reload();

	if (group) {
		connect(m_group, &Group::sInsertedSet, this,
			[this]() {
			qDebug() << "reload after insertion";
			reload();
		});
		connect(m_group, &Group::sRemovedSet, this,
			[this]() {
			qDebug() << "reload after remove";
			reload();
		});
		connect(m_group, &Group::sReset, this,
			[this]() {
			qDebug() << "reload after reset";
			reload();
		});
		connect(m_group, &Group::sItemsMoved, this,
			[this](auto stuff) {
			qDebug() << "moving items";
			moveItems(stuff);
		});
	}
}

Group * GroupScrollBox::getGroup() const
{
	return m_group;
}

ScrollBoxItem *GroupScrollBox::createItem(osg::Node *)
{
	return nullptr;
}
