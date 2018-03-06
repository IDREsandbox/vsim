#include <QScrollBar>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <vector>
#include <algorithm>

#include "GroupScrollBox.h"

GroupScrollBox::GroupScrollBox(QWidget* parent)
	: HorizontalScrollBox(parent),
	m_group(nullptr)
{
}

void GroupScrollBox::reload()
{
	clear();
	if (!m_group) return;

	// create items
	if (m_group) {
		std::vector<std::pair<size_t, osg::Node*>> items;
		for (uint i = 0; i < m_group->getNumChildren(); i++) {
			items.push_back({ i, m_group->child(i) });
		}
		insertItemsForIndices(items);
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
		connect(m_group, &Group::destroyed, this,
			[this]() {
			m_group = nullptr;
			setGroup(nullptr); // clear stuff
		});
		connect(m_group, &Group::sInsertedSet, this,
			[this](auto insertions) {
			insertItemsForIndices(insertions);
		});
		connect(m_group, &Group::sRemovedSet, this,
			[this](const std::vector<size_t> &removals) {
			removeItems(removals);
		});
		connect(m_group, &Group::sReset, this,
			[this]() {
			reload();
		});
		connect(m_group, &Group::sItemsMoved, this,
			[this](auto stuff) {
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

void GroupScrollBox::insertItemsForIndices(const std::vector<std::pair<size_t, osg::Node*>>& insertions)
{
	// create items
	if (m_group) {
		std::vector<std::pair<size_t, ScrollBoxItem*>> items;
		for (auto &pair : insertions) {
			ScrollBoxItem *item = createItem(pair.second);
			items.push_back({ pair.first, item });
		}
		insertItems(items);
	}
}