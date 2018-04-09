#ifndef GROUPTEMPLATE_H
#define GROUPTEMPLATE_H

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <QUndoStack>
#include "Util.h"

class GroupSignals : public QObject {
	Q_OBJECT
public:

signals:
	void sAboutToReset();
	void sReset();

	void sItemsMoved(std::vector<std::pair<size_t, size_t>>); // <from, to> items sorted by from
	void sEdited(const std::set<size_t> &edited); // for proxy/big listeners

	// single
	// void sNew(int index); // inserted node at index
	// void sDelete(int index); // deleted node at index
	// void sSet(int index); // changed (set) node at index

	// continuous, baseline, used for qt
	void sAboutToInsert(size_t index, size_t count);
	void sInserted(size_t index, size_t count);
	void sAboutToRemove(size_t index, size_t count);
	void sRemoved(size_t index, size_t count);

	// arbitrary indices, used for proxy
	void sAboutToInsertMulti(const std::vector<size_t> &);
	void sInsertedMulti(const std::vector<size_t> &);
	void sAboutToRemoveMulti(const std::vector<size_t> &);
	void sRemovedMulti(const std::vector<size_t> &);

	// cant use types because of template
	//// arbitrary indices, used for er proxy, group
	//void sAboutToInsertMulti(const std::vector<std::pair<size_t, T*>> &);
	//void sInsertedMulti(const std::vector<std::pair<size_t, T*>> &);
	//void sAboutToRemoveMulti(const std::vector<size_t> &);
	//void sRemovedMulti(const std::vector<size_t> &);

	//// sets, used for canvas
	//void sAboutToInsertMultiP(const std::set<T*> &);
	//void sInsertedMultiP(const std::set<T*> &);
	//void sAboutToRemoveMultiP(const std::set<T*> &nodes);
	//void sRemovedMultiP(const std::set<T*> &nodes);
};


// Group is a collection of nodes which emits signals
//  on every change
// If dealing with single indices use insert/remove
// Dealing with sets of indices use insertSet/removeSet
// Dealing with sets of nodes use addP/removeP
template <class T>
class TGroup : public GroupSignals {
public:
	TGroup() {};

	// Use this instead of the osg one. For some reason the osg one isn't virtual or const
	virtual T *child(size_t index) const;
	virtual std::shared_ptr<T> childShared(size_t index) const;
	virtual void move(const std::vector<std::pair<size_t, size_t>>& mapping);
	virtual int indexOf(const T *node) const; // -1 if not found
	virtual void clear();
	virtual size_t size() const;

	typedef typename std::vector<std::shared_ptr<T>>::iterator iterator;
	iterator begin();
	iterator end();

	// simple
	virtual void append(std::shared_ptr<T> node);
	// continuous *
	virtual bool insert(size_t index, const std::vector<std::shared_ptr<T>> &nodes);
	virtual bool remove(size_t index, size_t count);
	// index based, multiple
	virtual void insertMulti(const std::vector<std::pair<size_t, std::shared_ptr<T>>> &children);
	virtual void removeMulti(const std::vector<size_t> &children);
	// pointer based, multiple
	//virtual void insertMultiP(const std::set<std::shared_ptr<T>> &children);
	//virtual void removeMultiP(const std::set<std::shared_ptr<T>> &children);

public: // commands
	class Test {
	};

private:
	std::vector<std::shared_ptr<T>> m_children;
};

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
		if (m_index < 0) m_index = m_group->size() - 1;
		m_group->remove(m_index, 1);
	}
	virtual void redo() override {
		if (m_index < 0) m_index = m_group->size();
		m_group->insert(m_index, {m_node});
	}
private:
	TGroup<T> *m_group;
	std::shared_ptr<T> m_node;
	int m_index;
};

// Deletes a node at index
//template <class T>
//class DeleteNodeCommand : public QUndoCommand {
//public:
//	DeleteNodeCommand(
//		TGroup<T> *group,
//		size_t index,
//		QUndoCommand *parent = nullptr)
//		: QUndoCommand(parent),
//		m_group(group),
//		m_index(index) {
//		m_node = group->child(index);
//	}
//	virtual void undo() override {
//		m_group->insert(m_index, {m_node});
//	}
//	virtual void redo() override {
//		m_group->remove(m_index, 1);
//	}
//private:
//	TGroup<T> *m_group;
//	std::shared_ptr<T> m_node;
//	size_t m_index;
//};

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
			if (n) m_nodes.push_back({i, n});
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
template <class T>
class EditCommand : public QUndoCommand {
public:
	EditCommand(
		TGroup<T> *group,
		const std::set<size_t> &nodes,
		QUndoCommand *parent = nullptr)
		: QUndoCommand(parent),
		m_group(group),
		m_nodes(nodes) {
	}
	virtual void undo() override {
		QUndoCommand::undo();
		m_group->sEdited(m_nodes); // emit after changes
	}
	virtual void redo() override {
		QUndoCommand::redo();
		m_group->sEdited(m_nodes);
	}
private:
	TGroup<T> *m_group;
	std::set<size_t> m_nodes;
};

#include "GroupTemplate.inl"

#endif