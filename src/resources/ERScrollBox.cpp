#include "resources/ERScrollBox.h"
#include "resources/ERScrollItem.h"
#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QDebug>

ERScrollBox::ERScrollBox(QWidget * parent)
	: FastScrollBox(parent),
	m_group(nullptr),
	m_old_top(nullptr)
{
	setAutoFillBackground(false);
	m_view->setAutoFillBackground(false);
	m_view->setStyleSheet("background:transparent;");
	//m_view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	//m_view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	m_view->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
	// put updates on a timer?

	QTimer *t = new QTimer(this);
	t->setInterval(15);
	t->start();
	connect(t, &QTimer::timeout, this, [this]() {
		//m_view->updateSceneRect(m_view->rect);
		////m_view->updateScene();
		//m_view->SmartViewportUpdate;
		// visible rect
		QRect vr = m_view->viewport()->geometry();
		QRectF sr = m_view->mapToScene(vr).boundingRect();
		m_view->updateScene({ sr });
		m_view->update();
	});

	// top changed signal
	connect(selection(), &StackObject<FastScrollItem*>::sChanged, this, [this]() {
		EResource *new_top = nullptr;
		emit sSelectionChanged();
		do {
			auto *item = selection()->top();
			if (!item) break;
			ERScrollItem *eitem = qgraphicsitem_cast<ERScrollItem*>(item);
			if (!eitem) break;
			new_top = eitem->resource();
		} while (false);

		if (new_top != m_old_top) {
			m_old_top = new_top;
			emit sTopChanged(new_top);
		}
	});
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
			//setGroup(nullptr); // clear stuff
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

void ERScrollBox::addToSelection(EResource * res, bool top)
{
	ERScrollItem *item = remap(res);
	if (!item) return;
	if (top) {
		m_selection->add(item);
	}
	else {
		m_selection->addAt(item, 0);
	}
}

void ERScrollBox::deselect(EResource * res) const
{
	auto it = m_map.find(res);
	if (it == m_map.end()) return;
	ERScrollItem *item = it->second;
	selection()->remove(item);
}

bool ERScrollBox::has(EResource * res) const
{
	return m_map.count(res) > 0;
}

bool ERScrollBox::setTop(EResource * res)
{
	// map to item
	auto it = m_map.find(res);
	if (it == m_map.end()) return false;

	ERScrollItem *item = it->second;
	selection()->add(item);
	return true;
}

void ERScrollBox::setSelection(const std::vector<EResource*> &sel)
{
	auto item_selection = selection()->toVector();
	
	selection()->clear();

	for (auto *res : sel) {
		auto it = m_map.find(res);
		if (it == m_map.end()) continue;
		auto *item = it->second;
		selection()->add(item);
	}
}

std::vector<EResource*> ERScrollBox::getSelection() const
{
	std::vector<EResource*> out;
	auto item_selection = selection()->toVector();

	for (auto *item : item_selection) {
		ERScrollItem *eri = qgraphicsitem_cast<ERScrollItem*>(item);
		if (eri && eri->resource()) {
			out.push_back(eri->resource());
		}
	}
	return out;
}

void ERScrollBox::itemMouseDoubleClickEvent(FastScrollItem * item, QGraphicsSceneMouseEvent * event)
{
	if (event->button() == Qt::LeftButton
		&& !(event->modifiers() & Qt::CTRL)) {
		item->setSelected(true);
		event->accept();
		emit sOpen();
	}
}

ERScrollItem * ERScrollBox::remap(EResource * res) const
{
	auto it = m_map.find(res);
	if (it == m_map.end()) return nullptr;
	return it->second;
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
