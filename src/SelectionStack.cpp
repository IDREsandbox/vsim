#include "SelectionStack.h"
#include <algorithm>
#include <QDebug>

SelectionStack::SelectionStack(QObject *parent)
	: QObject(parent)
{
}

// manipulation
void SelectionStack::add(int x)
{
	push(x);
	emit sAdded(x);
	emit sChanged();
}
void SelectionStack::remove(int x)
{
	removeInternal(x);
	emit sRemoved(x);
	emit sChanged();
}
void SelectionStack::clear()
{
	m_stack.clear();
	m_selection.clear();
	emit sReset();
	emit sChanged();
}
void SelectionStack::select(int x)
{
	std::set<int> removed(m_stack.begin(), m_stack.end());

	m_stack.clear();
	m_selection.clear();
	push(x);

	for (auto i : removed) {
		if (i != x)	emit sRemoved(i);
	}
	emit sAdded(x);
	emit sChanged();
}

bool SelectionStack::selectIfNot(int index)
{
	if (has(index)) return false;
	select(index);
	return true;
}

void SelectionStack::rangeSelect(int to)
{
	int from = last();
	if (from < 0 && to < 0) {
		return;
	}
	if (from == to) {
		return;
	}
	if (from < 0 && to >= 0) {
		select(to);
		return;
	}

	// select increasing, 2, 3, 4
	if (to > from) {
		for (int i = from; i <= to; i++) {
			push(i);
			emit sAdded(i);
		}
	}
	// select decreasing, 4, 3, 2
	else {
		for (int i = from; i >= to; i--) {
			push(i);
			emit sAdded(i);
		}
	}
	emit sChanged();
}

size_t SelectionStack::size() const
{
	return m_stack.size();
}

bool SelectionStack::has(int index) const
{
	return m_selection.find(index) != m_selection.end();
}

bool SelectionStack::empty() const
{
	return m_stack.empty();
}

int SelectionStack::last() const
{
	if (m_stack.size() == 0) return -1;
	return *m_stack.rbegin();
}

// assignment
void SelectionStack::set(const SelectionData &d)
{
	clear();
	for (auto i : d) {
		push(i);
		emit sAdded(i);
	}
	emit sChanged();
}

SelectionData SelectionStack::data() const
{
	SelectionData d(m_stack.begin(), m_stack.end());
	return d;
}

std::set<int> SelectionStack::toSet() const
{
	std::set<int> s(m_stack.begin(), m_stack.end());
	return s;
}

std::set<size_t> SelectionStack::toUSet() const
{
	return std::set<size_t>(m_stack.begin(), m_stack.end());
}

SelectionData SelectionStack::dataFromSet(const std::set<int> &set)
{
	SelectionData d(set.begin(), set.end());
	return d;
}

bool SelectionStack::push(int index)
{
	if (index < 0) return false;
	auto it = m_selection.find(index);
	if (it != m_selection.end()) return false;

	// add to last selected stack
	m_stack.push_back(index);
	auto stack_it = m_stack.end();
	--stack_it;
	StackItem s = { index, stack_it };

	// add to map
	m_selection.insert({ index, s });
	return true;
}

bool SelectionStack::removeInternal(int index)
{
	if (index < 0) return false;
	auto it = m_selection.find(index);
	if (it == m_selection.end()) return false;

	// remove from stack
	m_stack.erase(it->second.stack_it);

	// remove from map
	m_selection.erase(it);

	return true;
}
