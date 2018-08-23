#include "GroupTemplate.h" // doesn't actually do anything, just for intellisense
#include "VecUtil.h"

template<class T>
inline TGroup<T>& TGroup<T>::operator=(const TGroup & other)
{
	GroupSignals::operator=(other);
	clear();
	for (auto sp : other) {
		auto nsp = std::make_shared<T>();
		*nsp = *sp;
		append(nsp);
	}
	return *this;
}

template <class T>
T *TGroup<T>::child(size_t i) const
{
	if (i >= size()) return nullptr;
	return m_children[i].get();
}

template<class T>
inline std::shared_ptr<T> TGroup<T>::childShared(size_t index) const
{
	if (index >= size()) return nullptr;
	return m_children[index];
}

template <class T>
void TGroup<T>::move(const std::vector<std::pair<size_t, size_t>>& mapping)
{
	VecUtil::multiMove(&m_children, mapping);
	emit sItemsMoved(mapping);
}

template <class T>
int TGroup<T>::indexOf(const T* node) const
{
	auto it = std::find_if(m_children.begin(), m_children.end(),
		[node](const std::shared_ptr<T> &sp) { return sp.get() == node; });
	if (it == m_children.end()) return -1;
	return it - m_children.begin();
}

template <class T>
void TGroup<T>::clear()
{
	emit sAboutToReset();
	m_children.clear();
	emit sReset();
}

template<class T>
size_t TGroup<T>::size() const
{
	return m_children.size();
}

template<class T>
inline typename TGroup<T>::iterator TGroup<T>::begin()
{
	return m_children.begin();
}

template<class T>
inline typename TGroup<T>::iterator TGroup<T>::end()
{
	return m_children.end();
}

template<class T>
inline typename TGroup<T>::citerator TGroup<T>::begin() const
{
	return m_children.cbegin();
}

template<class T>
inline typename TGroup<T>::citerator TGroup<T>::end() const
{
	return m_children.cend();
}

template<class T>
inline void TGroup<T>::append(std::shared_ptr<T> node)
{
	insert(size(), { node });
}

template <class T>
bool TGroup<T>::insert(size_t index, const std::vector<std::shared_ptr<T>>& nodes)
{
	if (index > m_children.size()) return false;
	//if (restrictedToCurrent()) return false;

	// other signal structures
	std::set<std::shared_ptr<T>> pointers(nodes.begin(), nodes.end());
	std::vector<std::pair<size_t, std::shared_ptr<T>>> list(nodes.size());
	for (size_t i = 0; i < nodes.size(); i++) {
		list[i] = { i + index, nodes[i] };
	}
	std::vector<size_t> indices(nodes.size());
	std::iota(indices.begin(), indices.end(), index);


	//emit sAboutToInsertMuliP(pointers);
	//emit sAboutToInsertMulti(list);
	emit sAboutToInsert(index, nodes.size());

	auto it = m_children.begin() + index;
	m_children.insert(it, nodes.begin(), nodes.end());

	emit sInserted(index, nodes.size());
	emit sInsertedMulti(indices);
	//emit sInsertedMultiP(pointers);
	return true;
}

template <class T>
bool TGroup<T>::remove(size_t index, size_t count)
{
	if (index + count > m_children.size()) return false;
	//if (restrictedToCurrent()) return false;

	// other signal structures
	std::set<std::shared_ptr<T>> pointers;
	std::vector<size_t> list;
	for (size_t i = 0; i < count; i++) {
		pointers.insert(childShared(index + i));
		list.push_back(index + i);
	}
	//emit sAboutToRemoveMultiP(pointers);
	emit sAboutToRemoveMulti(list);
	emit sAboutToRemove(index, count);

	auto begin = m_children.begin() + index;
	auto end = m_children.begin() + (index + count);
	m_children.erase(begin, end);

	emit sRemoved(index, count);
	emit sRemovedMulti(list);
	//emit sRemovedMultiP(pointers);
	return true;
}

template <class T>
void TGroup<T>::insertMulti(const std::vector<std::pair<size_t, std::shared_ptr<T>>> &insertions)
{
	if (insertions.size() == 0) return;

	// convert to (index, vec<Node*>) list
	std::vector<std::pair<size_t, std::vector<std::shared_ptr<T>>>> clumps;
	clumps = VecUtil::clumpify2(insertions);

	// insert in forward order
	for (const auto &i_clump : clumps) {
		insert(i_clump.first, i_clump.second);
	}
}

template <class T>
void TGroup<T>::removeMulti(const std::vector<size_t> &indices)
{
	if (indices.size() == 0) return;

	// (index, count)
	auto ic_pairs = VecUtil::clumpify3(indices);

	// remove clumps in reverse order
	for (auto i_c = ic_pairs.rbegin(); i_c != ic_pairs.rend(); ++i_c) {
		remove(i_c->first, i_c->second);
	}
}

//template <class T>
//void TGroup<T>::insertMultiP(const std::set<std::shared_ptr<T>> &children)
//{
//	// convert pointers to indices
//	std::vector<std::pair<size_t, std::shared_ptr<T>>> nodes;
//	size_t n = size();
//	size_t i = 0;
//	for (auto node : children) {
//		nodes.push_back(std::make_pair(n + i, node));
//		i++;
//	}
//	insertMulti(nodes);
//}
//
//template <class T>
//void TGroup<T>::removeMultiP(const std::set<std::shared_ptr<T>> &children)
//{
//	std::vector<size_t> removals;
//	// convert pointers to indices
//	for (size_t i = 0; i < size(); i++) {
//		if (children.find(childShared(i)) != children.end()) {
//			// found, add
//			removals.push_back(i);
//		}
//	}
//	// sort and send
//	std::sort(removals.begin(), removals.end());
//	removeMulti(removals);
//}
