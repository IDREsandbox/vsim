#include "Group.h"
#include <QDebug>

osg::Node *Group::child(unsigned int i)
{
	return getChild(i);
}

Group::AddNodeCommand::AddNodeCommand(
	Group *group,
	osg::Node *node,
	int index,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_group(group),
	m_node(node),
	m_index(index)
{
}

void Group::AddNodeCommand::undo() {
	uint index;
	if (m_index < 0) index = m_group->getNumChildren() - 1;
	else index = (uint)m_index;
	m_group->removeChild(index);
	m_group->sDelete(index);
}
void Group::AddNodeCommand::redo() {
	uint index;
	if (m_index < 0) index = m_group->getNumChildren();
	else index = (uint)m_index;
	m_group->insertChild(index, m_node);
	m_group->sNew(index);
}

Group::DeleteNodeCommand::DeleteNodeCommand(
	Group *group,
	int index,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_group(group),
	m_index(index)
{
	m_node = group->child(index);
}

void Group::DeleteNodeCommand::undo() {
	m_group->insertChild(m_index, m_node);
	m_group->sNew(m_index);
}

void Group::DeleteNodeCommand::redo() {
	m_group->removeChild(m_index);
	m_group->sDelete(m_index);
}

Group::MoveNodesCommand::MoveNodesCommand(
	Group * group,
	std::vector<std::pair<int, int>> mapping,
	QUndoCommand * parent)
	: QUndoCommand(parent),
	m_group(group),
	m_mapping(mapping)
{
}

void Group::MoveNodesCommand::undo()
{
	// flip the from and to
	std::vector<std::pair<int, int>> reverse_mapping;
	for (auto i : m_mapping) {
		reverse_mapping.push_back(std::make_pair(i.second, i.first));
	}
	move(reverse_mapping);
}

void Group::MoveNodesCommand::redo()
{
	move(m_mapping);
}

void Group::MoveNodesCommand::move(std::vector<std::pair<int, int>> mapping) {
	// remove the items and add them to this vector, <node, to>
	std::vector<std::pair<osg::ref_ptr<osg::Node>, int>> removed;

	// sort by the from and remove in reverse order <from, to>
	std::sort(mapping.begin(), mapping.end(),
		[](auto &left, auto &right) { return left.first < right.first; });

	for (int i = mapping.size() - 1; i >= 0; i--) {
		removed.push_back(
			std::make_pair(m_group->getChild(mapping[i].first), mapping[i].second));
		m_group->removeChild(mapping[i].first);
	}

	// sort
	std::sort(removed.begin(), removed.end(),
		[](auto &left, auto &right) { return left.second < right.second; });

	// now readd them in forward order
	for (auto i : removed) {
		m_group->insertChild(i.second, i.first);
	}

	m_group->sItemsMoved(mapping);
}