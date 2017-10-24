#ifndef SELECTION_H
#define SELECTION_H

#include <QObject>
#include <set>

// Abstract representation of a selection

class Selection : public QObject {
	Q_OBJECT
public:
	Selection();

	// selection - these set selection, lastSelected, and emit events
	virtual void setSelection(std::set<int> set, int last);
	void addToSelection(int);
	void removeFromSelection(int);
	void clearSelection();
	void select(int);

	// lastSelected must be in the selection
	int getLastSelected();
	void setLastSelected(int);
	const std::set<int>& getSelection();

	bool isSelected(int);

signals:
	void sAdded(int); // an index was added
	void sRemoved(int); // an index was removed
	void sChanged(); // anything changes

private:
	//bool m_multiselect;

	std::set<int> m_selection;
	int m_last_selected;
};

#endif // SELECTION_H