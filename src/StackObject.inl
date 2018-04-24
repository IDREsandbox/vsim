#ifndef STACKOBJECT_INL
#define STACKOBJECT_INL
#include "StackObject.h"

template<class T>
inline void StackObject<T>::add(T x)
{
	auto it = m_set.find(x);
	if (it != m_set.end()) {
		if (x == top()) return; // already at the top
		m_stack.erase(std::remove(
			m_stack.begin(), m_stack.end(), x), m_stack.end());
		m_stack.push_back(x);
	}
	else {
		m_set.insert(x);
		m_stack.push_back(x);
	}
	emit sAdded();
	emit sChanged();
}

template<class T>
inline void StackObject<T>::addAt(T x, int index)
{
	auto it = m_set.find(x);
	
	int target_index;
	if (index < 0) {
		// size 4, insert at -1 => insert at 3
		target_index = size() + index;
	}
	if (target_index < 0) return;
	if (target_index > size()) return;
	if (it != m_set.end()) {
		return;
	}
	else {
		m_stack.insert(m_stack.begin() + target_index, x);
	}
	emit sAdded();
	emit sChanged();
}

template<class T>
inline void StackObject<T>::remove(T x)
{
	auto set_it = m_set.find(x);
	if (set_it == m_set.end()) return;

	auto it = std::find(m_stack.begin(), m_stack.end(), x);
	size_t index = it - m_stack.begin();

	m_stack.erase(it);
	m_set.erase(set_it);
	m_removed = x;
	emit sRemoved();
	emit sChanged();
}

template<class T>
inline void StackObject<T>::clear()
{
	m_stack.clear();
	m_set.clear();
	emit sReset();
	emit sChanged();
}

template<class T>
inline bool StackObject<T>::has(T item) const
{
	return m_set.find(item) != m_set.end();
}

template<class T>
inline T StackObject<T>::at(size_t index) const
{
	if (index >= m_stack.size()) return T();
	return m_stack[index];
}

template<class T>
inline size_t StackObject<T>::size() const
{
	return m_stack.size();
}

template<class T>
inline T StackObject<T>::top() const
{
	if (size() == 0) return T();
	return *m_stack.rbegin();
}

template<class T>
inline T StackObject<T>::removed() const
{
	return m_removed;
}

template<class T>
inline std::vector<T> StackObject<T>::toVector() const
{
	return m_stack;
}

template<class T>
inline std::set<T> StackObject<T>::toSet() const
{
	return m_set;
}

#endif