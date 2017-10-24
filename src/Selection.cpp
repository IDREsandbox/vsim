#include "Selection.h"
#include <algorithm>
#include <QDebug>

Selection::Selection() 
	: m_last_selected(-1), m_selection({})
{
}
void Selection::setSelection(std::set<int> new_set, int last)
{
	// items being removed
	std::set<int> removed;
	std::set_difference(m_selection.begin(), m_selection.end(), new_set.begin(), new_set.end(),
		std::inserter(removed, removed.begin()));

	// items being added
	std::set<int> added;
	std::set_difference(new_set.begin(), new_set.end(), m_selection.begin(), m_selection.end(),
		std::inserter(added, added.begin()));
	
	m_selection = new_set;
	if (m_selection.empty()) {
		m_last_selected = -1;
	}
	// in case of invalid stuff, just set automatically
	else if (m_selection.find(last) == m_selection.end() || last == -1) {
		qDebug() << "Invalid set of last selection in setSelection(), setting to the end";
		m_last_selected = *m_selection.rbegin();
	}
	else {
		m_last_selected = last;
	}

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
	m_last_selected = x;
	if (m_selection.find(x) != m_selection.end()) return;
	m_selection.insert(x);
	emit sAdded(x);
}

void Selection::removeFromSelection(int x)
{
	if (m_selection.find(x) == m_selection.end()) return;
	m_selection.erase(x);
	if (m_selection.rbegin() != m_selection.rend()) {
		m_last_selected = *m_selection.rbegin();
	}
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
