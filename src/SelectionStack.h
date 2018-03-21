#ifndef SELECTIONSTACK_H
#define SELECTIONSTACK_H

#include <QObject>
#include <set>
#include <vector>

typedef std::vector<int> SelectionData;

// Ordered selection of indexes.
// last() is used like the old getLastSelected().
class SelectionStack : public QObject {
	Q_OBJECT
public:
	SelectionStack(QObject *parent = nullptr);

	// manipulation
	// emits one sChanged signal each
	void add(int index);
	void remove(int index);
	void clear();
	void select(int index);
	bool selectIfNot(int index);
	void rangeSelect(int to);

	// info
	size_t size() const;
	bool has(int index) const;
	bool empty() const;
	int last() const;

	// assignment/conversion
	// kind of lazy on this one, emits a bunch of signals
	void set(const SelectionData &d);
	SelectionData data() const;
	std::set<int> toSet() const;
	static SelectionData dataFromSet(const std::set<int> &set);

signals:
	void sAdded(int); // an index was added
	void sRemoved(int); // an index was removed
	void sReset();
	void sChanged(); // anything changes

private:
	bool push(int index);
	bool removeInternal(int index);

	struct StackItem {
		int index;
		std::list<int>::iterator stack_it;
	};
	std::list<int> m_stack;
	std::map<int, StackItem> m_selection;
};

#endif