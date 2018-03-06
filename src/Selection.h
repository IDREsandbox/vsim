#ifndef SELECTION_H
#define SELECTION_H

#include <QObject>
#include <set>

// Abstract representation of a selection
class Selection : public QObject {
	Q_OBJECT
public:
	Selection(QObject *parent = nullptr);

	// selection - these set selection, lastSelected, and emit events
	virtual void set(std::set<int> set, int last);
	void add(int); // also sets last selected
	void remove(int);
	void clear();
	void select(int); // removes everything else
	bool selectIfNot(int); // selects if not already, returns if cleared

	void shiftSelect(int);

	// lastSelected must be in the selection
	// -1 means none
	int getLastSelected();
	const std::set<int>& getSelection();

	bool contains(int);

signals:
	void sAdded(int); // an index was added
	void sRemoved(int); // an index was removed
	void sChanged(); // anything changes
	void sSingleSelect(int);

public:
	// If sel items are deleted from total, then this returns the item after
	// sel. If there are no items after then it returns the previous.
	// If there is nothing left then -1
	static int nextAfterDelete(int size, std::set<int> selection);
	static std::set<int> nextSelectionAfterDelete(int size, std::set<int> selection);

private:
	//bool m_multiselect;

	std::set<int> m_selection;
	int m_last_selected;
};

#endif // SELECTION_H