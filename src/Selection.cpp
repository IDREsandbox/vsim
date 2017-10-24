#include "Selection.h"
#include <algorithm>

Selection::Selection() {
}
void Selection::setSelection(std::set<int> new_set, int last)
{
	// items being removed
	std::set<int> removed;
	//auto it = std::set_difference(new_set.begin(), new_set.end(), m_selection.begin(), m_selection.end(), removed.begin());

	std::set<int> a;
	std::set<int> b;
	std::set<int> c;
	auto it = std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.end()));

	// items being added
	std::set<int> added;
	//std::set_difference(m_selection.begin(), m_selection.end(), new_set.begin(), new_set.end(), added.begin());
	
	m_selection = new_set;
	m_last_selected = last;

	// emit signals
	for (int i : removed) {
		emit sRemoved(i);
	}
	for (int i : added) {
		emit sAdded(i);
	}
	emit sChanged();

}

void Selection::addToSelection(int x)
{
	if (m_selection.find(x) != m_selection.end()) return;
	m_selection.insert(x);
	emit sAdded(x);
}

void Selection::removeFromSelection(int x)
{
	if (m_selection.find(x) == m_selection.end()) return;
	m_selection.erase(x);
	emit sRemoved(x);
}

void Selection::clearSelection()
{
	setSelection({}, -1);
}

void Selection::select(int x)
{
	setSelection({x}, x);
}

int Selection::getLastSelected()
{
	return m_last_selected;
}

const std::set<int>& Selection::getSelection()
{
	return m_selection;
}

bool Selection::isSelected(int x)
{
	return (m_selection.find(x) != m_selection.end());
}
