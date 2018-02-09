#include "Group.h"
#include <QDebug>
#include <unordered_set>
#include "Util.h"

osg::Node *Group::child(unsigned int i) const
{
	if (i >= getNumChildren()) return nullptr;
	return _children[i].get();
}

bool Group::addChild(osg::Node * child)
{
	return insertChild(getNumChildren(), child);
}

bool Group::insertChild(unsigned int index, Node *child)
{
	insertChildrenSet({ { index, child } });
	return true;
}

bool Group::removeChildren(unsigned int index, unsigned int numChildrenToRemove)
{
	//int top = std::min(index + numChildrenToRemove, getNumChildren());
	//bool result = osg::Group::removeChildren(index, numChildrenToRemove);
	//for (int i = top - 1; i >= (int)index; i--) {
	//	emit sDelete(i);
	//}
	//std::set<int> removed_set;
	//for (size_t i = 0; i < numChildrenToRemove; i++) {
	//	removed_set.insert(index + i);
	//}
	//emit sRemovedSet(removed_set);

	std::vector<size_t> removals(numChildrenToRemove);
	std::iota(removals.begin(), removals.end(), index);
	removeChildrenSet(removals);

	return true;
}

bool Group::setChild(unsigned int index, Node *child)
{
	bool result = osg::Group::setChild(index, child);
	emit sSet(index);
	return result;
}

void Group::move(const std::vector<std::pair<size_t, size_t>>& mapping)
{
	Util::multiMove(&_children, mapping);
	emit sItemsMoved(mapping);
}

int Group::indexOf(const osg::Node * node) const
{
	int x = getChildIndex(node);
	if (x == getNumChildren()) return -1;
	return x;
}

void Group::clear()
{
	emit sBeginReset();
	_children.clear();
	emit sReset();
}

void Group::insertChildrenSet(const std::vector<std::pair<size_t, osg::Node*>> &insertions)
{
	if (insertions.size() == 0) return;

	std::set<osg::Node*> set;
	for (auto &pair : insertions) {
		set.insert(pair.second);
	}

	emit sAboutToAddP(set);
	emit sAboutToInsertSet(insertions);

	std::vector<osg::Node*> new_list(_children.begin(), _children.end());
	Util::multiInsert(&new_list, insertions);
	// convert to refs, a direct _children.assign() causes refs crash and burn
	std::vector<osg::ref_ptr<Node>> refs(new_list.begin(), new_list.end());
	_children = refs;

	emit sAddedP(set);
	emit sInsertedSet(insertions);
}

void Group::removeChildrenSet(const std::vector<size_t> &indices)
{
	if (indices.size() == 0) return;

	// pointers
	std::set<osg::Node*> set;
	for (size_t i : indices) {
		set.insert(child(i));
	}

	emit sAboutToRemoveSet(indices);
	emit sAboutToRemoveP(set);

	std::vector<osg::Node*> new_list(_children.begin(), _children.end());
	Util::multiRemove(&new_list, indices);
	std::vector<osg::ref_ptr<Node>> refs(new_list.begin(), new_list.end());
	_children = refs;

	emit sRemovedSet(indices);
	emit sRemovedP(set);
}

void Group::addChildrenP(const std::set<osg::Node*> &children)
{
	// convert pointers to indices
	std::vector<std::pair<size_t, osg::Node*>> nodes;
	size_t i = getNumChildren();
	for (auto node : children) {
		nodes.push_back(std::make_pair(i, node));
	}
	insertChildrenSet(nodes);
}

void Group::removeChildrenP(const std::set<osg::Node*> &children)
{
	std::vector<size_t> removals;
	// convert pointers to indices
	for (size_t i = 0; i < getNumChildren(); i++) {
		if (children.find(child(i)) != children.end()) {
			// found, add
			removals.push_back(i);
		}
	}
	// sort and send
	std::sort(removals.begin(), removals.end());
	removeChildrenSet(removals);
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
}
void Group::AddNodeCommand::redo() {
	uint index;
	if (m_index < 0) index = m_group->getNumChildren();
	else index = (uint)m_index;
	m_group->insertChild(index, m_node);
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
}

void Group::DeleteNodeCommand::redo() {
	m_group->removeChild(m_index);
}

Group::MoveNodesCommand::MoveNodesCommand(
	Group * group,
	const std::vector<std::pair<size_t, size_t>> &mapping,
	QUndoCommand * parent)
	: QUndoCommand(parent),
	m_group(group),
	m_mapping(mapping)
{
}
void Group::MoveNodesCommand::undo()
{
	// flip the from and to
	std::vector<std::pair<size_t, size_t>> reverse_mapping;
	for (auto i : m_mapping) {
		reverse_mapping.push_back(std::make_pair(i.second, i.first));
	}
	m_group->move(reverse_mapping);
}
void Group::MoveNodesCommand::redo()
{
	m_group->move(m_mapping);
}

Group::InsertSetCommand::InsertSetCommand(
	Group *group,
	const std::map<int, osg::Node*> &nodes,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_group(group)
{
	for (auto pair : nodes) {
		int index = pair.first;
		osg::Node *node = pair.second;
		m_nodes[index] = osg::ref_ptr<osg::Node>(node);
	}
}
void Group::InsertSetCommand::undo()
{
	// convert set to vector
	std::vector<size_t> nodes;
	for (auto &pair : m_nodes) {
		nodes.push_back(pair.first);
	}
	m_group->removeChildrenSet(nodes);
}
void Group::InsertSetCommand::redo()
{
	// convert map to vector of pairs
	std::vector<std::pair<size_t, osg::Node*>> list;
	list.assign(m_nodes.begin(), m_nodes.end());
	m_group->insertChildrenSet(list);
}

Group::RemoveSetCommand::RemoveSetCommand(
	Group *group,
	const std::set<int> &nodes,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_group(group)
{
	for (auto index : nodes) {
		osg::Node *node = m_group->child(index);
		m_nodes[index] = osg::ref_ptr<osg::Node>(node);
	}
}
void Group::RemoveSetCommand::undo()
{
	std::vector<std::pair<size_t, osg::Node*>> list;
	list.assign(m_nodes.begin(), m_nodes.end());
	m_group->insertChildrenSet(list);
}
void Group::RemoveSetCommand::redo()
{
	std::vector<size_t> nodes;
	for (auto &pair : m_nodes) {
		nodes.push_back(pair.first);
	}
	m_group->removeChildrenSet(nodes);
}

Group::AddNodesPCommand::AddNodesPCommand(
	Group *group,
	const std::set<osg::Node*> &nodes,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_group(group)
{
	// keep references
	for (auto node : nodes) {
		m_nodes.insert(osg::ref_ptr<osg::Node>(node));
	}
}
void Group::AddNodesPCommand::undo()
{
	// convert from refs to ptrs
	std::set<osg::Node*> nodes;
	for (auto node_ref : m_nodes) {
		nodes.insert(node_ref);
	}
	m_group->removeChildrenP(nodes);
}
void Group::AddNodesPCommand::redo()
{
	std::set<osg::Node*> nodes;
	for (auto node_ref : m_nodes) {
		nodes.insert(node_ref);
	}
	m_group->addChildrenP(nodes);
}

Group::RemoveNodesPCommand::RemoveNodesPCommand(
	Group *group,
	const std::set<osg::Node*> &nodes,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_group(group)
{
	// keep references
	for (auto node : nodes) {
		m_nodes.insert(osg::ref_ptr<osg::Node>(node));
	}
}
void Group::RemoveNodesPCommand::undo()
{
	// convert from refs to ptrs
	std::set<osg::Node*> nodes;
	for (auto node_ref : m_nodes) {
		nodes.insert(node_ref);
	}
	m_group->addChildrenP(nodes);
}
void Group::RemoveNodesPCommand::redo()
{
	std::set<osg::Node*> nodes;
	for (auto node_ref : m_nodes) {
		nodes.insert(node_ref);
	}
	m_group->removeChildrenP(nodes);
}

Group::EditCommand::EditCommand(Group * group, const std::set<int>& nodes, QUndoCommand * parent)
	: QUndoCommand(parent),
	m_group(group),
	m_nodes(nodes)
{
}
void Group::EditCommand::undo()
{
	QUndoCommand::undo();
	m_group->sEdited(m_nodes);
}
void Group::EditCommand::redo()
{
	QUndoCommand::redo();
	m_group->sEdited(m_nodes);
}
