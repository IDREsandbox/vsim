#ifndef GROUPTEMPLATE_H
#define GROUPTEMPLATE_H

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <QUndoStack>

#include "GroupSignals.h"

// Group is a collection of nodes which emits signals
//  on every change
// If dealing with single indices use insert/remove
// Dealing with sets of indices use insertSet/removeSet
// Dealing with sets of nodes use addP/removeP
template <class T>
class TGroup : public GroupSignals {
public:
	TGroup(QObject *parent = nullptr) : GroupSignals(parent) {};
	TGroup &operator= (const TGroup &other);

	// Use this instead of the osg one. For some reason the osg one isn't virtual or const
	virtual T *child(size_t index) const;
	virtual std::shared_ptr<T> childShared(size_t index) const;
	virtual void move(const std::vector<std::pair<size_t, size_t>>& mapping);
	virtual int indexOf(const T *node) const; // -1 if not found
	virtual void clear();
	virtual size_t size() const;

	using iterator = typename std::vector<std::shared_ptr<T>>::iterator;
	using citerator = typename std::vector<std::shared_ptr<T>>::const_iterator;
	iterator begin();
	iterator end();
	citerator begin() const;
	citerator end() const;

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

private:
	std::vector<std::shared_ptr<T>> m_children;
};

#include "GroupTemplate.inl"

#endif