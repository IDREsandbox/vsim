#ifndef GROUPCOMMANDS_H
#define GROUPCOMMANDS_H

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <QUndoStack>
#include "GroupTemplate.h"
#include "Command.h"

template <class T>
class MoveNodesCommand : public QUndoCommand {
public:
	// mapping .first is from, .second is to
	MoveNodesCommand(TGroup<T> *group,
		const std::vector<std::pair<size_t, size_t>> &mapping,
		QUndoCommand *parent = nullptr)
		: QUndoCommand(parent),
		m_group(group),
		m_mapping(mapping) {
	}
	void undo() {
		// flip the from and to
		std::vector<std::pair<size_t, size_t>> reverse_mapping;
		for (auto i : m_mapping) {
			reverse_mapping.push_back(std::make_pair(i.second, i.first));
		}
		m_group->move(reverse_mapping);
	}
	void redo() {
		m_group->move(m_mapping);
	}
private:
	TGroup<T> *m_group;
	std::vector<std::pair<size_t, size_t>> m_mapping;
};


template <class T>
class AddNodeCommand : public QUndoCommand {
public:
	AddNodeCommand(
		TGroup<T> *group,
		std::shared_ptr<T> node,
		int index = -1,
		QUndoCommand *parent = nullptr)
		: QUndoCommand(parent),
		m_group(group),
		m_node(node),
		m_index(index) {
	}
	virtual void undo() override {
		if (m_index < 0) m_index = (int)m_group->size() - 1;
		m_group->remove(m_index, 1);
	}
	virtual void redo() override {
		if (m_index < 0) m_index = (int)m_group->size();
		m_group->insert(m_index, { m_node });
	}
private:
	TGroup<T> *m_group;
	std::shared_ptr<T> m_node;
	int m_index;
};

// Deletes a node at index
template <class T>
class DeleteNodeCommand : public QUndoCommand {
public:
	DeleteNodeCommand(
		TGroup<T> *group,
		size_t index,
		QUndoCommand *parent = nullptr)
		: QUndoCommand(parent),
		m_group(group),
		m_index(index) {
		m_node = group->child(index);
	}
	virtual void undo() override {
		m_group->insert(m_index, { m_node });
	}
	virtual void redo() override {
		m_group->remove(m_index, 1);
	}
private:
	TGroup<T> *m_group;
	std::shared_ptr<T> m_node;
	size_t m_index;
};

// index based, multiple
//template <class T>
//class InsertMultiCommand : public QUndoCommand {
//public:
//	InsertMultiCommand(
//		TGroup<T> *group,
//		const std::map<size_t, T*> &nodes,
//		QUndoCommand *parent = nullptr)
//		: QUndoCommand(parent),
//		m_group(group) {
//	}
//	virtual void undo() override {
//		std::vector<size_t> indices(m_nodes.size(), 0);
//		for (auto &pair : m_nodes) {
//			indices.push_back(pair.first);
//		}
//		m_group->removeMulti(indices);
//	}
//	virtual void redo() override {
//		m_group->insertMulti(m_nodes);
//		// get shared pointers
//		for (auto &pair : m_nodes) {
//			m_nodes.push_back(
//				{pair.first,}
//			);
//		}
//	}
//private:
//	TGroup<T> *m_group;
//	std::vector<std::pair<size_t, std::shared_ptr<T>>> m_nodes;
//};

// index based, multiple
template <class T>
class RemoveMultiCommand : public QUndoCommand {
public:
	RemoveMultiCommand(
		TGroup<T> *group,
		const std::set<size_t> &nodes,
		QUndoCommand *parent = nullptr)
		: QUndoCommand(parent),
		m_group(group) {
		m_nodes.reserve(nodes.size());
		for (size_t i : nodes) {
			std::shared_ptr<T> n = group->childShared(i);
			if (n) m_nodes.push_back({ i, n });
		}
	}
	virtual void undo() override {
		m_group->insertMulti(m_nodes);
	}
	virtual void redo() override {
		std::vector<size_t> indices;
		indices.reserve(m_nodes.size());
		for (auto &pair : m_nodes) {
			indices.push_back(pair.first);
		}
		m_group->removeMulti(indices);
	}
private:
	TGroup<T> *m_group;
	std::vector<std::pair<size_t, std::shared_ptr<T>>> m_nodes;
};

// pointer based, multiple
//template <class T>
//class InsertPCommand : public QUndoCommand {
//public:
//	InsertPCommand(
//		TGroup<T> *group,
//		const std::set<T*> &nodes,
//		QUndoCommand *parent = nullptr)
//		: QUndoCommand(parent),
//		m_group(group) {
//		for (auto node : nodes) {
//			m_nodes.insert(std::shared_ptr<T>(node));
//		}
//	}
//	virtual void undo() override {
//		std::set<T*> nodes;
//		for (auto &node_ref : m_nodes) {
//			nodes.insert(node_ref);
//		}
//		m_group->removeMultiP(nodes);
//	}
//	virtual void redo() override {
//		std::set<T*> nodes;
//		for (auto &node_ref : m_nodes) {
//			nodes.insert(node_ref);
//		}
//		m_group->insertMultiP(nodes);
//	}
//private:
//	TGroup<T> *m_group;
//	std::set<std::shared_ptr<T>> m_nodes;
//};

// pointer based, multiple
//template <class T>
//class RemoveNodesPCommand : public QUndoCommand {
//public:
//	RemoveNodesPCommand(
//		TGroup<T> *group,
//		const std::set<T*> &nodes,
//		QUndoCommand *parent = nullptr)
//		: QUndoCommand(parent),
//		m_group(group) {
//		// find nodes and make shared pointers
//		for (size_t i = 0; i < group->size(); i++) {
//			if (nodes.find(group->child(i)) != nodes.end()) {
//				m_nodes.insert(group->childShared(i));
//			}
//		}
//	}
//	virtual void undo() override {
//		m_group->insertMultiP(m_nodes);
//	}
//	virtual void redo() override {
//		m_group->removeMultiP(m_nodes);
//	}
//private:
//	TGroup<T> *m_group;
//	std::set<std::shared_ptr<T>> m_nodes;
//};

// causes sEdited to be emitted on undo/redo
class EditCommand : public QUndoCommand {
public:
	EditCommand(
		GroupSignals *group,
		const std::set<size_t> &nodes,
		Command::When when = Command::ON_BOTH,
		QUndoCommand *parent = nullptr)
		: QUndoCommand(parent),
		m_group(group),
		m_nodes(nodes),
		m_when(when) {
	}
	virtual void undo() override {
		QUndoCommand::undo();
		if (m_when & Command::ON_UNDO) {
			m_group->sEdited(m_nodes);
		}
	}
	virtual void redo() override {
		QUndoCommand::redo();
		if (m_when & Command::ON_REDO) {
			m_group->sEdited(m_nodes);
		}
	}
private:
	GroupSignals *m_group;
	std::set<size_t> m_nodes;
	Command::When m_when;
};

#endif