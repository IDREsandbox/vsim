#include "Group.h"
#include <QDebug>

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
	bool result = osg::Group::insertChild(index, child);
	emit sNew(index);
	return result;
}

bool Group::removeChildren(unsigned int index, unsigned int numChildrenToRemove)
{
	int top = std::min(index + numChildrenToRemove, getNumChildren());
	bool result = osg::Group::removeChildren(index, numChildrenToRemove);
	for (int i = top - 1; i >= (int)index; i--) {
		emit sDelete(i);
	}
	return result;
}

bool Group::setChild(unsigned int index, Node *child)
{
	bool result = osg::Group::setChild(index, child);
	emit sSet(index);
	return result;
}

void Group::move(const std::vector<std::pair<int, int>>& mapping)
{
	unsigned int size = getNumChildren();

	std::vector<osg::ref_ptr<osg::Node>> buffer(size, nullptr);
	std::vector<bool> moved(size, false);

	// perform movements in buffer
	for (auto pair : mapping) {
		if (buffer[pair.second] != nullptr) {
			qWarning() << "Move failed - two elements move to the same index";
			return;
		}
		buffer[pair.second] = child(pair.first);
		moved[pair.first] = true;
	}

	// fill in the holes with the rest
	unsigned int out = 0;
	unsigned int in = 0;
	while (out < size && in < size) {
		// output already taken
		if (buffer[out] != nullptr) {
			out++;
		}
		// input already moved
		else if (moved[in] == true) {
			in++;
		}
		else {
			buffer[out] = child(in);
			out++;
			in++;
		}
	}

	// write back the differences
	for (unsigned int i = 0; i < size; i++) {
		if (child(i) != buffer[i].get()) {
			setChild(i, buffer[i].get());
		}
	}

	emit sItemsMoved(mapping);
}

int Group::indexOf(const osg::Node * node) const
{
	int x = getChildIndex(node);
	if (x == getNumChildren()) return -1;
	return x;
}


void Group::insertChildrenSet(const std::map<int, osg::Node*>& children)
{
	emit sAboutToInsertSet(children);
	for (auto &pair : children) {
		int index = pair.first;
		osg::Node *node = pair.second;
		insertChild(index, node);
	}
	emit sInsertedSet(children);
}

void Group::removeChildrenSet(const std::set<int>& children)
{
	emit sAboutToRemoveSet(children);
	for (auto it = children.rbegin(); it != children.rend(); ++it) {
		int index = *it;
		removeChild(index, 1);
	}
	emit sRemovedSet(children);
}

void Group::addChildrenP(const std::set<osg::Node*> &children)
{
	emit sAboutToAddP(children);
	qDebug() << "addchildren" << children.size();
	for (auto node : children) {
		addChild(node);
	}

	emit sAddedP(children);
}

void Group::removeChildrenP(const std::set<osg::Node*> &children)
{
	qDebug() << "remove children p";
	emit sAboutToRemoveP(children);

	for (auto node : children) {
		qDebug() << "removing child" << node;
		removeChild(node);
	}

	emit sRemovedP(children);
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
	std::set<int> nodes;
	for (auto pair : m_nodes) {
		nodes.insert(pair.first);
	}
	m_group->removeChildrenSet(nodes);
}
void Group::InsertSetCommand::redo()
{
	std::map<int, osg::Node*> nodes;
	for (auto pair : m_nodes) {
		nodes[pair.first] = pair.second;
	}
	m_group->insertChildrenSet(nodes);
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
	std::map<int, osg::Node*> nodes;
	for (auto pair : m_nodes) {
		nodes[pair.first] = pair.second;
	}
	m_group->insertChildrenSet(nodes);
}
void Group::RemoveSetCommand::redo()
{
	std::set<int> nodes;
	for (auto pair : m_nodes) {
		nodes.insert(pair.first);
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
