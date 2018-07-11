#ifndef GROUPSCROLLBOX_INL
#define GROUPSCROLLBOX_INL

#include "GroupScrollBox.h"
#include "ScrollBoxItem.h"

template <class T>
GroupScrollBox<T>::GroupScrollBox(QWidget* parent)
	: HorizontalScrollBox(parent),
	m_group(nullptr)
{
}

template <class T>
void GroupScrollBox<T>::reload()
{
	clear();

	if (!m_group) return;

	std::vector<std::pair<size_t, ScrollBoxItem*>> insertions;
	for (size_t i = 0; i < m_group->size(); i++) {
		T *node = m_group->child(i);
		ScrollBoxItem *item = createItem(node);
		insertions.push_back(std::make_pair(i, item));
	}
	insertItems(insertions);
}

template <class T>
void GroupScrollBox<T>::setGroup(TGroup<T> *group)
{
	// disconnect incoming signals if already connected to a narrative
	if (m_group != nullptr) disconnect(m_group, 0, this, 0);

	m_group = group;

	reload();

	if (group) {
		connect(m_group, &GroupSignals::destroyed, this,
			[this]() {
			m_group = nullptr;
			clear();
		});
		connect(m_group, &GroupSignals::sInsertedMulti, this,
			[this](std::vector<size_t> list) {

			std::vector<std::pair<size_t, ScrollBoxItem*>> insertions;
			for (auto i : list) {
				ScrollBoxItem *item = createItem(m_group->child(i));
				insertions.push_back(std::make_pair(i, item));
			}
			insertItems(insertions);
		});
		connect(m_group, &GroupSignals::sAboutToRemoveMulti, this,
			[this](const std::vector<size_t> &removals) {
			removeItems(removals);
		});
		connect(m_group, &GroupSignals::sAboutToReset, this,
			[this]() {
			clear();
		});
		connect(m_group, &GroupSignals::sReset, this,
			[this]() {
			reload();
		});
		connect(m_group, &GroupSignals::sItemsMoved, this,
			[this](auto stuff) {
			moveItems(stuff);
		});
	}
}

template <class T>
TGroup<T> *GroupScrollBox<T>::group() const
{
	return m_group;
}

template <class T>
ScrollBoxItem *GroupScrollBox<T>::createItem(T *)
{
	return new ScrollBoxItem;
}

#endif