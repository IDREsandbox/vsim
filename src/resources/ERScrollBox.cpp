#include "resources/ERScrollBox.h"
#include "resources/ERScrollItem.h"
#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include <QGraphicsSceneMouseEvent>

ERScrollBox::ERScrollBox(QWidget * parent)
	: FastScrollBox(parent),
	m_group(nullptr)
{
}

void ERScrollBox::setGroup(TGroup<EResource> *group)
{
	// disconnect incoming signals if already connected to a narrative
	if (m_group != nullptr) disconnect(m_group, 0, this, 0);

	m_group = group;

	reload();

	if (group) {
		connect(m_group, &GroupSignals::destroyed, this,
			[this]() {
			m_group = nullptr;
			setGroup(nullptr); // clear stuff
		});
		connect(m_group, &GroupSignals::sInsertedMulti, this,
			[this](std::vector<size_t> list) {
			insertForIndices(list);
		});
		connect(m_group, &GroupSignals::sAboutToRemoveMulti, this,
			[this](const std::vector<size_t> &removals) {
			removeItems(removals);
			for (auto index : removals) {
				m_map.erase(m_group->child(index));
			}
		});
		connect(m_group, &GroupSignals::sAboutToReset, this,
			[this]() {
			clear();
			m_map.clear();
		});
		connect(m_group, &GroupSignals::sReset, this,
			[this]() {
			reload();
		});
		connect(m_group, &GroupSignals::sItemsMoved, this,
			[this](auto stuff) {
			// moveItems(stuff);
		});
	}
}

void ERScrollBox::reload()
{
	clear();
	if (!m_group) return;

	m_map.clear();

	std::vector<size_t> all(m_group->size());
	std::iota(all.begin(), all.end(), 0);
	insertForIndices(all);
}

void ERScrollBox::setSelection(const std::vector<EResource*>& sel)
{
	auto item_selection = selection()->toVector();
	
	for (auto *item : item_selection) {

	}
}

std::vector<EResource*> ERScrollBox::getSelection() const
{
	std::vector<EResource*> out;
	auto item_selection = selection()->toVector();

	for (auto *item : item_selection) {
		out.push_back;
		ERScrollItem *item = dynamic_cast<ERScrollItem*>(item);
		if (item && item->) {
			out.push_back(item->)
		}
	}
}

void ERScrollBox::itemMouseDoubleClickEvent(FastScrollItem * item, QGraphicsSceneMouseEvent * event)
{
	if (event->button() == Qt::LeftButton) {
		item->setSelected(true);
		event->accept();
		emit sOpen();
		
	}
}

void ERScrollBox::insertForIndices(const std::vector<size_t>& ind)
{
	std::vector<std::pair<size_t, FastScrollItem*>> insertions;
	for (auto i : ind) {
		EResource *res = m_group->child(i);
		ERScrollItem *item = createItem(m_group->child(i));
		m_map[res] = item;
		insertions.push_back({ i, item });
	}
	insertItems(insertions);
}

ERScrollItem * ERScrollBox::createItem(EResource * res)
{
	auto *item = new ERScrollItem();
	item->setER(res);
	item->setCat(res->category());
	return item;
}
