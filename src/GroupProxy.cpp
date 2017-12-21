#include "GroupProxy.h"
#include <QDebug>
#include <map>
#include <vector>
#include <algorithm>

GroupProxy::GroupProxy()
	: GroupProxy(nullptr)
{
}

GroupProxy::GroupProxy(Group *base)
	: m_base(nullptr)
{
	setBase(base);
}

Group * GroupProxy::getBase() const
{
	return m_base;
}

void GroupProxy::setBase(Group *base)
{
	if (m_base != nullptr) disconnect(m_base, 0, this, 0);

	m_base = base;
	if (base == nullptr) return;

	// listen to base changes and adjust
	connect(base, &Group::sNew, this, [this](int index){
		// everything after the new item is shifted right 1
		for (int i = 0; i < m_map_to_base.size(); i++) {
			if (m_map_to_base[i] >= index) {
				m_map_to_base[i] += 1;
			}
		}
	});

	connect(base, &Group::sDelete, this, [this](int index){
		// everything after the deleted item is shifted left 1
		for (int i = m_map_to_base.size() - 1; i >= 0; i--) {
			if (m_map_to_base[i] == index) {
				// this item got deleted
				m_map_to_base.erase(m_map_to_base.begin() + i);
				emit sDelete(i);
				// keep index the same
				continue;
			}
			else if (m_map_to_base[i] > index) {
				m_map_to_base[i] -= 1;
			}
		}
	});

	connect(base, &Group::sItemsMoved, this, [this](
		std::vector<std::pair<int, int>> map_list) {
		// don't move anything here, but fix up the indices
		// build a map
		std::map<int, int> mapping;
		for (auto pair : map_list) {
			mapping.insert(pair);
		}
		// convert any of the changes
		for (int i = 0; i < m_map_to_base.size(); i++) {
			auto it = mapping.find(m_map_to_base[i]);
			if (it != mapping.end()) {
				m_map_to_base[i] = it->second;
			}
		}
	});
}

int GroupProxy::mapToBase(int i) const
{
	if (i >= m_map_to_base.size()) return -1;
	return m_map_to_base[i];
}

void GroupProxy::setMapToBase(const std::vector<int>& mapping)
{
	// delete everything
	for (int i = 0; i < m_map_to_base.size(); i++) {
		emit sDelete(i);
	}

	m_map_to_base.clear();

	// insert new stuff
	for (int i = 0; i < mapping.size(); i++) {
		m_map_to_base.push_back(mapping[i]);
		emit sNew(i);
	}
}

bool GroupProxy::insertChild (unsigned int index, osg::Node *child)
{
	if (!m_base) return false;
	qWarning() << "Insertion into a group proxy - item added to end";
	return m_base->addChild(child);
}

bool GroupProxy::removeChildren (unsigned int index, unsigned int numChildrenToRemove)
{
	if (!m_base) return false;
	std::vector<int> delete_me;
	for (unsigned int i = 0; i < numChildrenToRemove; i++) {
		delete_me.push_back(mapToBase(index));
	}
	std::sort(delete_me.begin(), delete_me.end());
	for (int i = delete_me.size() - 1; i >= 0; i--) {
		m_base->removeChild(delete_me[i]);
	}
	return true;
}

osg::Node *GroupProxy::child(unsigned int index) const
{
	if (!m_base) return nullptr;
	return m_base->child(mapToBase(index));
}

unsigned int GroupProxy::getNumChildren() const
{
	if (!m_base) return 0;
	return m_map_to_base.size();
}
