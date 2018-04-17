#ifndef GROUPTEMPLATE_H
#define GROUPTEMPLATE_H

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <QUndoStack>

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
	//void sAboutToInsertMulti(const std::vector<size_t> &); // unused
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

#include "GroupTemplate.inl"

#endif