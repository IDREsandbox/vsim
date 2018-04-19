#ifndef STACKOBJECT_H
#define STACKOBJECT_H

#include <QObject>
#include <set>
#include <vector>

class StackSignals : public QObject {
	Q_OBJECT
public:
	StackSignals(QObject *parent = nullptr)
		: QObject(parent) {}

signals:
	void sAdded(); // something added to the top
	void sRemoved(); // call removed() to get what
	void sReset();
	void sChanged(); // anything changes
};

template <class T>
class StackObject : public StackSignals {
public:
	void add(T);
	void remove(T);
	void clear();
	//void singleSelect(FastScrollItem *);
	//bool selectIfNot(FastScrollItem *);

	bool has(T item) const;
	T at(size_t index) const;
	size_t size() const;
	T top() const;
	T removed() const;

	std::vector<T> toVector() const;
	std::set<T> toSet() const;

private:
	T m_removed;
	std::vector<T> m_stack;
	std::set<T> m_set;
};


#include "StackObject.inl"
#endif