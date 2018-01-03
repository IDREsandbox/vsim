#include "Selection.h"
#include <algorithm>
#include <QDebug>

Selection::Selection(QObject *parent)
	: QObject(parent),
	m_last_selected(-1),
	m_selection({})
{
}

void Selection::set(std::set<int> new_set, int last)
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
		qDebug() << "Invalid set of last selection in set(), setting to the end";
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

void Selection::add(int x)
{
	m_last_selected = x;
	if (m_selection.find(x) != m_selection.end()) return;
	m_selection.insert(x);
	emit sAdded(x);
}

void Selection::remove(int x)
{
	if (m_selection.find(x) == m_selection.end()) return;
	m_selection.erase(x);
	if (m_selection.rbegin() != m_selection.rend()) {
		m_last_selected = *m_selection.rbegin();
	}
	emit sRemoved(x);
}

void Selection::clear()
{
	set({}, -1);
}

void Selection::select(int x)
{
	set({x}, x);
}

void Selection::selectIfNot(int index)
{
	if (!contains(index)) {
		select(index);
	}
	else {
		add(index);
	}
}

void Selection::shiftSelect(int next)
{
	int prev = m_last_selected;
	if (prev < 0 && next < 0) {
		return;
	}
	if (prev < 0 && next >= 0) {
		select(next);
		return;
	}
	m_last_selected = next;
	int left = std::min(prev, next);
	int right = std::max(prev, next);
	for (int i = left; i <= right; i++) {
		add(i);
	}
}

int Selection::getLastSelected()
{
	return m_last_selected;
}

const std::set<int>& Selection::getSelection()
{
	return m_selection;
}

bool Selection::contains(int x)
{
	return (m_selection.find(x) != m_selection.end());
}

int Selection::nextAfterDelete(int total, std::set<int> selection)
{
	// figure out the selection after deleting
	int first_index = *selection.begin();
	int remaining = total - (int)selection.size();
	int next_selection;
	if (remaining == 0) { // everyone's gone
		next_selection = -1;
	}
	else if (remaining >= first_index + 1) {
		next_selection = first_index; // select next non-deleted item
	}
	else {
		next_selection = first_index - 1; // select the previous item
	}
	return next_selection;
}

std::set<int> Selection::nextSelectionAfterDelete(int size, std::set<int> selection)
{
	std::set<int> next_selection = {};
	int next = nextAfterDelete(size, selection);
	if (next >= 0) next_selection = {next};
	return next_selection;
}
