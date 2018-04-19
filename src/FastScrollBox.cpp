#include "FastScrollBox.h"
#include <QHBoxLayout>
#include <QGraphicsRectItem>
#include <QDebug>
#include <QResizeEvent>
#include <QGraphicsSceneMouseEvent>
#include <QElapsedTimer>

static void execMenu(const QPoint &pos) {
	QMenu menu;
	menu.addAction(new QAction("heyyy"));
	menu.addAction(new QAction("weowww"));
	menu.exec(pos);
}

FastScrollBox::FastScrollBox(QWidget * parent)
	: QFrame(parent),
	m_spacing(8)
{
	setWindowTitle("fast scroll box");
	m_scene = new Scene(this, this);
	// this is way faster, for 1k+ items
	m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);

	m_view = new QGraphicsView(this);
	m_view->setScene(m_scene);
	m_view->show();
	m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	resize(400, 100);

	QHBoxLayout *layout = new QHBoxLayout(this);
	this->setLayout(layout);
	layout->addWidget(m_view);

	m_selection = new StackObject<FastScrollItem*>;
	connect(m_selection, &StackSignals::sRemoved, this, [this]() {
		m_selection->removed()->setSelected(false);
	});
	connect(m_selection, &StackSignals::sAdded, this, [this]() {
		m_selection->top()->setSelected(true);
	});
	connect(m_selection, &StackSignals::sChanged, this, [this]() {
		//m_selection->top()->setTop(true);
	});

	//m_view->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect(m_view, &QWidget::customContextMenuRequested, [this](const QPoint &pos) {
	//	//execMenu(m_view->mapToGlobal(pos));
	//});
	//m_view->setDragMode(QGraphicsView::RubberBandDrag);
}

void FastScrollBox::setMenu(QMenu * menu)
{
	m_menu = menu;
}

void FastScrollBox::setItemMenu(QMenu * menu)
{
	m_item_menu = menu;
}

void FastScrollBox::setSpacing(int spacing)
{
	m_spacing = spacing;
	refresh();
}

void FastScrollBox::refresh()
{
	QElapsedTimer t; t.start();
	// viewport height
	int vh = m_view->viewport()->height();
	vh = std::max(vh, 50);

	int x = 0;
	x += m_spacing;
	for (auto item : m_items) {
		item->setPos(x, 0);
		item->setSize(QSizeF(vh, vh));
		x += vh;
		x += m_spacing;
	}

	m_scene->setSceneRect(0, 0, x, vh);
	QRectF min(QPointF(0, 0), m_view->viewport()->size());
	m_view->setSceneRect(m_scene->sceneRect().united(min));
}

void FastScrollBox::clear()
{
	for (auto *item : m_items) {
		delete item;
	}
	m_items.clear();
	m_selection->clear();
}

void FastScrollBox::selectAll(bool select)
{
	for (auto item : m_items) {
		item->setSelected(select);
	}
	if (!select) {
		emit sSelectionCleared();
	}
}

StackObject<FastScrollItem*>* FastScrollBox::selection()
{
	return m_selection;
}

void FastScrollBox::insertItems(const std::vector<std::pair<size_t, FastScrollItem*>>& insertions)
{
	for (const auto &pair : insertions) {
		auto *item = pair.second;
		m_scene->addItem(item);
		m_items.insert(m_items.begin() + pair.first, item);
		connect(item, &FastScrollItem::sSelected, [this, item](bool selected) {
			if (selected) {
				m_selection->add(item);
			}
			else {
				m_selection->remove(item);
			}
		});
		connect(item, &FastScrollItem::sMousePressEvent,
			[this, item](auto *event) {
			itemMousePressEvent(item, event);
		});
		connect(item, &FastScrollItem::sContextMenuEvent,
			[this](auto *event) {
			if (m_item_menu) {
				event->accept();
				m_item_menu->exec(event->screenPos());
			} 
		});
		connect(item, &FastScrollItem::sMouseDoubleClickEvent,
			[this, item](auto *event) {
			itemMouseDoubleClickEvent(item, event);
		});
	}

	refresh();
	//multiInsert(&m_items, insertions);
}

void FastScrollBox::removeItems(const std::vector<size_t> indices)
{
	for (auto i : indices) {
		m_scene->removeItem(m_items[i]);
		m_selection->remove(m_items[i]);
		delete m_items[i];
	}
	std::set<size_t> ind(indices.begin(), indices.end());
	for (int i = m_items.size() - 1; i >= 0; i--) {
		if (ind.count(i) > 0) m_items.erase(m_items.begin() + i);
	}
	//multiRemove();
	refresh();
}

void FastScrollBox::removeSelected()
{
	auto set = m_selection->toSet();
	auto rit = std::stable_partition(m_items.begin(), m_items.end(),
		[&set](FastScrollItem *item) {
		return set.count(item) == 0; // keep if not selected
	});
	size_t rindex = rit - m_items.begin();
	std::vector<size_t> ind(m_items.size() - rindex);
	std::iota(ind.begin(), ind.end(), rindex);
	removeItems(ind);
}

size_t FastScrollBox::itemCount() const
{
	return m_items.size();
}

FastScrollItem * FastScrollBox::item(size_t index) const
{
	if (index >= m_items.size()) return nullptr;
	return m_items[index];
}

void FastScrollBox::resizeEvent(QResizeEvent * event)
{
	QFrame::resizeEvent(event);

	// minimum: viewport size
	// typical val: scene rect
	//QRectF min(QPointF(0, 0), m_view->viewport()->size());
	//m_view->setSceneRect(m_scene->sceneRect().united(min));

	refresh();
}

void FastScrollBox::itemMousePressEvent(FastScrollItem * item, QGraphicsSceneMouseEvent * event)
{
	if (event->isAccepted()) return;
	if (event->button() == Qt::LeftButton) {
		if (event->modifiers() & Qt::ControlModifier) {
			// ctrl - add/remove from selection
			if (m_selection->has(item)) {
				m_selection->remove(item);
			}
			else {
				m_selection->add(item);
			}
		}
		else if (event->modifiers() & Qt::ShiftModifier) {
			//check top
			if (m_selection->size() == 0) {
				m_selection->add(item);
			}
			else {

				int from = std::find(m_items.begin(), m_items.end(),
					m_selection->top()) - m_items.begin();
				int to = std::find(m_items.begin(), m_items.end(), item)
					- m_items.begin();

				if (to > from) {
					for (int i = from; i <= to; i++) {
						m_selection->add(m_items[i]);
					}
				}
				else {
					for (int i = from; i >= to; i--) {
						m_selection->add(m_items[i]);
					}
				}
			}
		}
		else {
			singleSelect(item);
		}
		event->accept();
	}
	else if (event->button() == Qt::RightButton) {
		if (!m_selection->has(item)) {
			//qDebug() << "single select";
			singleSelect(item);
		}
		else {
			m_selection->add(item);
		}
	}
	emit sTouch();
}

void FastScrollBox::itemMouseDoubleClickEvent(FastScrollItem * item, QGraphicsSceneMouseEvent * event)
{
//	emit sTouch();
}

void FastScrollBox::singleSelect(FastScrollItem * item)
{
	item->setSelected(true);
	for (auto *other : m_items) {
		if (other != item) {
			qDebug() << "ss desel" << other->m_number;
			other->setSelected(false);
		}
	}
	//m_selection->clear();
	//m_selection->add(item);
	emit sSelectionCleared();
}

FastScrollItem::FastScrollItem()
	: QGraphicsObject(nullptr)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
}
void FastScrollItem::setSize(const QSizeF &size)
{
	if (size == m_size) return;
	prepareGeometryChange();
	m_size = size;
	onResize();
}
void FastScrollItem::setNumber(int number)
{
	m_number = number;
}

void FastScrollItem::paint(QPainter *painter,
	const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setBrush(QColor(50, 50, 100));
	painter->setPen(QPen(Qt::NoPen));
	painter->drawRect(QRectF(QPointF(0, 0), m_size));
}

QRectF FastScrollItem::boundingRect() const
{
	return QRectF(QPointF(0, 0), m_size);
}

QVariant FastScrollItem::itemChange(GraphicsItemChange change, const QVariant & value)
{
	if (change == QGraphicsItem::ItemSelectedChange) {
	}
	else if (change == QGraphicsItem::ItemSelectedHasChanged) {
		bool sel = value.toBool();
		onSelect(sel);
		emit sSelected(sel);
	}
	return QGraphicsItem::itemChange(change, value);
}

void FastScrollItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	event->ignore(); // for some reason this gets accepted before
	emit sMousePressEvent(event);
	//QGraphicsObject::mousePressEvent(event);
}

void FastScrollItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
	event->ignore();
	emit sMouseDoubleClickEvent(event);
	if (event->isAccepted()) return;
	QGraphicsObject::mouseDoubleClickEvent(event);
}

void FastScrollItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	event->ignore();
}

void FastScrollItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
	emit sContextMenuEvent(event);
	QGraphicsObject::contextMenuEvent(event);
}

void FastScrollItem::onResize()
{
	update();
}

void FastScrollItem::onSelect(bool selected)
{
	update();
}

FastTextItem::FastTextItem(QGraphicsItem *parent)
	: QGraphicsItem(parent)
{
}

void FastTextItem::setBoundingRect(qreal x, qreal y, qreal w, qreal h)
{
	prepareGeometryChange();
	m_rect = QRectF(x, y, w, h);
}

void FastTextItem::setText(const QString &inText) {
	m_text = inText;
	update();
}

void FastTextItem::setColor(const QColor &color)
{
	m_color = color;
	update();
}

void FastTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QTextOption opt;
	opt.setWrapMode(QTextOption::WordWrap);
	opt.setAlignment(Qt::AlignCenter);
	painter->setPen(m_color);
	painter->drawText(boundingRect(), m_text, opt);
}

QRectF FastTextItem::boundingRect() const
{
	return m_rect;
}

FastScrollBox::Scene::Scene(FastScrollBox *box, QObject * parent)
	: QGraphicsScene(parent)
{
	m_box = box;
}

void FastScrollBox::Scene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	QGraphicsScene::mousePressEvent(event);

	if (!event->isAccepted() && event->button() == Qt::LeftButton) {
		m_box->selectAll(false);
		event->accept();
	}
}

void FastScrollBox::Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	QGraphicsScene::mouseReleaseEvent(event);
}

void FastScrollBox::Scene::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
	QGraphicsScene::contextMenuEvent(event);
	if (!event->isAccepted() && m_box->m_menu) {
		m_box->m_menu->exec(event->screenPos());
		event->accept();
	}
}
