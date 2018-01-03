#include <QScrollBar>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <vector>
#include <algorithm>

#include "HorizontalScrollBox.h"
#include "ScrollBoxItem.h"
#include "Selection.h"

HorizontalScrollBox::HorizontalScrollBox(QWidget* parent)
	: QScrollArea(parent),
	m_menu(nullptr),
	m_item_menu(nullptr),
	m_group(nullptr),
	m_dragging(false),
	m_mouse_down(false),
	m_minimum_drag_dist(10)
{
	setObjectName("scrollArea");
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setWidgetResizable(true);

	m_scroll_area_widget = new QWidget(this);
	m_scroll_area_widget->setStyleSheet(
		"background-color: rgb(0,0,0,0);"
		//"QMenu {"
		//"	background-color: rgb(255,255,255);"
		//"}"
	);
	setWidget(m_scroll_area_widget);

	m_drop_highlight = new QWidget(m_scroll_area_widget);
	m_drop_highlight->setObjectName("dropHighlight");
	m_drop_highlight->setStyleSheet("background-color: rgb(255,255,255)");
	m_drop_highlight->hide();

	m_move_timer = new QTimer();
	m_move_timer->setInterval(15);
	m_move_timer->start();
	connect(m_move_timer, &QTimer::timeout, this, &HorizontalScrollBox::moveTimer);

	//m_height = m_scroll_area_widget->height();
	m_height = this->height();

	setAcceptDrops(true);

	m_selection = new Selection(this);
	connect(m_selection, &Selection::sAdded, this, [this](int i) {
		if (i >= m_items.size()) return;
		m_items[i]->select(true);
	});
	connect(m_selection, &Selection::sRemoved, this, [this](int i) {
		if (i >= m_items.size()) return;
		m_items[i]->select(false);
	});
}

void HorizontalScrollBox::addItem(ScrollBoxItem *item)
{
	insertItem(m_items.size(), item);
	horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
}

void HorizontalScrollBox::insertItem(int index, ScrollBoxItem *item)
{
	item->setParent(m_scroll_area_widget);
	// approximate geometry, will be fixed later on in refresh()
	item->setGeometry(0, 0, m_height, 2.5*m_height);
	item->widthFromHeight(m_height);
	m_items.insert(index, item);
	// steal all mouse events on items, alternatively we could use a filter
	connect(item, &ScrollBoxItem::sMousePressEvent, this, &HorizontalScrollBox::itemMousePressEvent);
	connect(item, &ScrollBoxItem::sMouseReleaseEvent, this, &HorizontalScrollBox::itemMouseReleaseEvent);

	item->show();
	refresh();
}

void HorizontalScrollBox::addNewItem()
{
	insertNewItem(m_items.size());
}

void HorizontalScrollBox::insertNewItem(uint index)
{
	m_selection->clear();
	osg::Node *node = nullptr;
	if (m_group != nullptr && index <= m_group->getNumChildren()) {
		node = m_group->child(index);
	}

	ScrollBoxItem *item = createItem(node); // create the item, virtual
	if (item == nullptr) return;
	HorizontalScrollBox::insertItem(index, item);
}

ScrollBoxItem *HorizontalScrollBox::getItem(int position)
{
	if (position < m_items.size()) {
		return m_items.at(position);
	}
	return nullptr;
}


void HorizontalScrollBox::clear()
{
	for (auto ptr : m_items) {
		delete ptr;
	}
	m_items.clear();
	m_selection->clear();

	if (m_group) disconnect(m_group, 0, this, 0);
	m_group = nullptr;

	refresh();
}

const std::set<int>& HorizontalScrollBox::getSelection()
{
	return m_selection->getSelection();
}

void HorizontalScrollBox::setSelection(const std::set<int>& set, int last)
{
	m_selection->set(set, last);
}

int HorizontalScrollBox::getLastSelected()
{
	return m_selection->getLastSelected();
}

void HorizontalScrollBox::deleteItem(int position)
{
	m_selection->clear();
	ScrollBoxItem* item = m_items.takeAt(position);
	delete item;
	refresh();
}

void HorizontalScrollBox::setMenu(QMenu * menu)
{
	m_menu = menu;
}

void HorizontalScrollBox::setItemMenu(QMenu * menu)
{
	m_item_menu = menu;
}

void HorizontalScrollBox::setMIMEType(const std::string & type)
{
	m_mime_type = QString::fromStdString(type);
}

void HorizontalScrollBox::setGroup(Group * group)
{
	// disconnect incoming signals if already connected to a narrative
	if (m_group != nullptr) disconnect(m_group, 0, this, 0);

	clear();
	m_group = group;
	if (group == nullptr) return;

	// listen to the insert and remove signals from group
	// so that we can add/remove from the slide box accordingly
	connect(m_group, &Group::sNew, this, &HorizontalScrollBox::insertNewItem);
	connect(m_group, &Group::sDelete, this, &HorizontalScrollBox::deleteItem);
	connect(m_group, &Group::sItemsMoved, this, &HorizontalScrollBox::moveItems);

	for (uint i = 0; i < group->getNumChildren(); i++) {
		insertNewItem(i);
	}
}

Group * HorizontalScrollBox::getGroup() const
{
	return m_group;
}

void HorizontalScrollBox::setSpacing(int space)
{
	m_spacing = space;
	refresh();
}

std::pair<int, float> HorizontalScrollBox::posToIndex(int px)
{
	int xpos = 0;
	if (m_items.size() == 0) {
		return std::make_pair(0, -.1f);
	}

	for (int i = 0; i < m_items.size(); i++) {
		// point is in the spacer?
		if (px < xpos + m_spacing) {
			return std::make_pair(i, -.1f);
		}

		ScrollBoxItem *o = m_items.at(i);
		int bwidth = o->widthFromHeight(m_height);

		// point is in the widget?
		if (px <= xpos + m_spacing + bwidth) {
			return std::make_pair(i, (px - xpos) / (float)(bwidth));
		}

		xpos += bwidth;
		xpos += m_spacing;
	}
	// at the end, return spacer at the end
	return std::make_pair(m_items.size(), -.1f);
}

void HorizontalScrollBox::refresh()
{
	for (int i = 0; i < m_items.size(); i++) {
		m_items[i]->setIndex(i);
	}

	if (m_items.size() == 0) {
		m_scroll_area_widget->setMinimumWidth(0);
		return;
	}

	// do we draw a drag-n-drop box?
	int spacer_index = -1; // spacer 0 is before item 0
	if (m_dragging) {
		m_drop_highlight->show();
		QPoint scrollspace = m_scroll_area_widget->mapFromGlobal(this->mapToGlobal(m_dragpos));
		//qDebug() << "local coords" << scrollspace << posToIndex(scrollspace.x());
		std::pair<int, float> pair = posToIndex(scrollspace.x());
		if (pair.second < .5) { // draw in this spacer
			spacer_index = pair.first;
		}
		else {
			spacer_index = pair.first + 1;
		}
		m_drag_index = spacer_index;
	}
	else {
		m_drop_highlight->hide();
	}


	int xpos = m_spacing;

	int bheight = m_height;

	for (int i = 0; i < m_items.size(); i++) {
		ScrollBoxItem *o = m_items.at(i);
		int bwidth = o->widthFromHeight(bheight);

		//QRect geom(xpos, margins.top(), bwidth, bheight);
		QRect geom(xpos, 0, bwidth, bheight);
		o->setGeometry(geom);

		if (m_dragging && spacer_index == i) {
			m_drop_highlight->setGeometry(xpos - m_spacing, 0, m_spacing, m_height);
		}
		xpos += bwidth;
		xpos += m_spacing;
	}
	if (m_dragging && spacer_index >= m_items.size()) {
		m_drop_highlight->setGeometry(xpos - m_spacing, 0, m_spacing, m_height);
	}

	m_scroll_area_widget->setMinimumWidth(xpos);
}

//void HorizontalScrollBox::forceSelect(int index)
//{
//	blockSignals(true);
//	select(index);
//	blockSignals(false);
//}

ScrollBoxItem * HorizontalScrollBox::createItem(osg::Node *)
{
	return nullptr;
}

void HorizontalScrollBox::moveItems(std::vector<std::pair<int, int>> mapping)
{
	std::vector<std::pair<ScrollBoxItem*,int>> removed;
	for (int i = mapping.size() - 1; i >= 0; i--) {
		removed.push_back(
			std::make_pair(m_items.takeAt(mapping[i].first), mapping[i].second));
	}
	// sort by destination
	std::sort(removed.begin(), removed.end(),
		[](auto &left, auto &right) { return left.second < right.second; });
	// now re-enter them in forward order
	for (auto i : removed) {
		m_items.insert(i.second, i.first);
	}
	refresh();
}

void HorizontalScrollBox::resizeEvent(QResizeEvent* event)
{
	// snapshot of current position and all that
	//int oldValue = horizontalScrollBar()->value();
	//QScrollBar *bar = horizontalScrollBar();
	//qDebug() << "resize event" << event->size().height();
	//qDebug() << "resize event before" << m_scroll_area_widget->height();
	m_height = event->size().height();
	refresh();

	// deal with other stuff
	QScrollArea::resizeEvent(event);

	//qDebug() << "resize event after" << m_scroll_area_widget->height();
	for (auto item : m_items) {
		item->widthFromHeight(m_scroll_area_widget->height());
	}
}

void HorizontalScrollBox::wheelEvent(QWheelEvent* event)
{
	horizontalScrollBar()->setValue(horizontalScrollBar()->value() - event->delta());
}

void HorizontalScrollBox::mouseMoveEvent(QMouseEvent * event)
{
	// If mouse down & beyond drag threshold then start dragging
	//qDebug() << "drag" << m_dragging;
	QPoint diff = event->globalPos() - m_mouse_down_pos;
	if (m_mime_type != ""
		&& diff.manhattanLength() > m_minimum_drag_dist
		&& m_dragging == false
		&& m_selection->getLastSelected() >= 0
		&& m_mouse_down
		) {

		// Begin drag
		QDrag *drag = new QDrag(this);
		ScrollBoxItem *item = getItem(getLastSelected());
		QPixmap pixmap(item->width(), item->height());
		item->render(&pixmap, QPoint(), QRegion(0, 0, item->width(), item->height()));
		drag->setPixmap(pixmap);

		QMimeData *data = new QMimeData;
		data->setData(m_mime_type, "");
		drag->setMimeData(data);
		drag->setHotSpot(event->globalPos() - item->mapToGlobal(QPoint(0, 0)));
		qDebug() << "drag start";

		// set dragging flag
		m_dragging = true;
		m_dragpos = event->pos();

		Qt::DropAction result = drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::MoveAction);
		qDebug() << "drop result" << result;
		m_dragging = false;
	}
	//m_mouse_down_pos = event->globalPos();
	//m_mouse_down = true;
}

void HorizontalScrollBox::mousePressEvent(QMouseEvent * event)
{
	m_selection->clear();
	emit sSelectionCleared();
	if (event->button() == Qt::RightButton) {
		if (m_menu != nullptr) {
			m_menu->exec(event->globalPos());
		}
	}
}

void HorizontalScrollBox::mouseReleaseEvent(QMouseEvent * event)
{
	m_mouse_down = false;
}

void HorizontalScrollBox::itemMousePressEvent(QMouseEvent * event, int index)
{
	if (event->button() == Qt::LeftButton) {
		int prev = m_selection->getLastSelected();
		if (event->type() == QEvent::MouseButtonDblClick) {
			//emit sDoubleClick();
			m_selection->add(index);
			return;
		}

		if (event->modifiers() & Qt::ControlModifier) {
			// ctrl - add/remove from selection
			if (m_selection->contains(index)) {
				m_selection->remove(index);
			}
			else {
				m_selection->add(index);
			}
		}
		else if (event->modifiers() & Qt::ShiftModifier) {
			// shift - range selection
			m_selection->shiftSelect(index);
		}
		else {
			// left click a selected item -> begin dragging
			//if (isSelected(index)) {
			bool cleared = m_selection->selectIfNot(index);
			if (cleared) emit sSelectionCleared();

			m_mouse_down_pos = event->globalPos();
			m_mouse_down = true;
		}
	}
	else if (event->button() == Qt::RightButton) {
		bool cleared = m_selection->selectIfNot(index);
		if (cleared) emit sSelectionCleared();

		// open menu
		if (m_item_menu) m_item_menu->exec(event->globalPos());
	}
}
void HorizontalScrollBox::itemMouseReleaseEvent(QMouseEvent * event, int index)
{
	if (event->button() == Qt::LeftButton) {
		if (!(event->modifiers() & Qt::ControlModifier)
			&& !(event->modifiers() & Qt::ShiftModifier)) {
			m_selection->select(index);
		}
	}
}

void HorizontalScrollBox::dragEnterEvent(QDragEnterEvent * event)
{
	if (event->mimeData()->hasFormat(m_mime_type)) {
		event->accept();
		//m_dragging = true;
		//m_dragpos = event->pos();
		//refresh();
	}
}

void HorizontalScrollBox::dragLeaveEvent(QDragLeaveEvent * event)
{
	if (m_dragging) {
		m_dragging = false;
		refresh();
	}
}

void HorizontalScrollBox::dragMoveEvent(QDragMoveEvent * event)
{
	if (event->mimeData()->hasFormat(m_mime_type)) {
		m_dragging = true;
		m_dragpos = event->pos();
		refresh();
	}
}

void HorizontalScrollBox::dropEvent(QDropEvent * event)
{
	if (event->mimeData()->hasFormat(m_mime_type)) {
		event->accept();
		std::set<int> selection = getSelection();
		// calculate the destination position
		// m_drag_index is the spacer index
		// new position is (spacer index) - (number of items removed before it)
		int dest_index = m_drag_index;
		for (auto i : selection) {
			if (i < m_drag_index) {
				dest_index--;
			}
		}

		// calculate the new order
		std::vector<int> new_order;
		for (int i = 0; i < m_items.size(); i++) {
			// if not in selection
			if (selection.find(i) == selection.end()) {
				new_order.push_back(i);
			}
		}
		new_order.insert(new_order.begin() + dest_index, selection.begin(), selection.end());

		// if the order hasn't changed, then don't emit a move signal
		if (!std::is_sorted(new_order.begin(), new_order.end())) {
			emit sMove(getSelection(), dest_index);
		}

		//qDebug() << "new order";
		//for (auto i : new_order) qDebug() << i;
	}
	m_dragging = false;
	m_drop_highlight->hide();
}
void HorizontalScrollBox::moveTimer()
{
	if (m_dragging) {
		int mx = m_dragpos.x();
		int my = m_dragpos.y();
		if (my > -20 && my < height() + 20) {
			QAbstractSlider *bar = horizontalScrollBar();
			if (mx < 40) {
				// scroll left
				// rate is linear with position
				int dx = (40 - mx) / 5.0;
				bar->setSliderPosition(bar->sliderPosition() - dx);
			}
			if (mx > width() - 40) {
				int dx = (mx - width() + 40) / 5.0;
				bar->setSliderPosition(bar->sliderPosition() + dx);

			}
		}
	}
}
