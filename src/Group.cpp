#include "Group.h"
#include <QDebug>

osg::Node *Group::child(unsigned int i)
{
	return getChild(i);
}

bool Group::addChild(osg::Node * child)
{
	qDebug() << "add child";
	return insertChild(getNumChildren(), child);
}

bool Group::insertChild(unsigned int index, Node *child)
{
	bool result = osg::Group::insertChild(index, child);
	qDebug() << "insert child";
	emit sNew(index);
	return result;
}

bool Group::removeChildren(unsigned int index, unsigned int numChildrenToRemove)
{
	int top = std::min(index + numChildrenToRemove, getNumChildren());
	bool result = osg::Group::removeChildren(index, numChildrenToRemove);
	for (int i = top - 1; i >= (int)index; i--) {
		emit sDelete(index + i);
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