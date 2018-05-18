#include <QScrollBar>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <vector>
#include <QDebug>
#include <algorithm>
#include <QElapsedTimer>
#include <QPaintEvent>

#include "HorizontalScrollBox.h"
#include "ScrollBoxItem.h"
#include "Selection.h"
#include "SelectionStack.h"
#include "VecUtil.h"

HSBScrollArea::HSBScrollArea(QWidget *parent)
	: QWidget(parent)
{
}
void HSBScrollArea::paintEvent(QPaintEvent *e) {
	e->accept();
}

HorizontalScrollBox::HorizontalScrollBox(QWidget* parent)
	: QWidget(parent),
	m_menu(nullptr),
	m_item_menu(nullptr),
	m_drag(nullptr),
	m_mouse_down(false),
	m_minimum_drag_dist(10),
	m_spacing(10)
{
	QGridLayout *layout = new QGridLayout(this);
	m_scroll = new QScrollArea(this);
	layout->addWidget(m_scroll, 0, 0);
	layout->setMargin(0);

	m_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_scroll->setWidgetResizable(true);
	m_scroll->viewport()->installEventFilter(this);
	m_scroll->setObjectName("scrollArea");
	m_scroll->setStyleSheet("#scrollArea { background:rgba(0, 0, 0, 0);}");

	m_scroll_area_widget = new HSBScrollArea();
	m_scroll->setWidget(m_scroll_area_widget);
	m_scroll_area_widget->show();
	m_scroll_area_widget->setStyleSheet(
		"background-color: rgb(0,0,0,0);"
		//"QMenu {"
		//"	background-color: rgb(255,255,255);"
		//"}"
	);

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
	setMIMEType("application/x-scrollitem");

	m_selection = new Selection(this);

	m_selection_stack = new SelectionStack(this);
	connect(m_selection_stack, &SelectionStack::sChanged, this, [this]() {
		int last = m_selection_stack->last();
		if (last >= 0) m_scroll->ensureWidgetVisible(m_items[last]);
	});
	connect(m_selection_stack, &SelectionStack::sAdded, this, [this](int i) {
		if (i >= m_items.size()) return;
		m_items[i]->select(true);
	});
	connect(m_selection_stack, &SelectionStack::sRemoved, this, [this](int i) {
		if (i >= m_items.size()) return;
		m_items[i]->select(false);
	});
	connect(m_selection_stack, &SelectionStack::sReset, this, [this]() {
		for (auto item : m_items) {
			item->select(false);
		}
	});

	refresh();
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
	m_selection_stack->clear();
	refresh();
}

SelectionStack * HorizontalScrollBox::selectionStack() const
{
	return m_selection_stack;
}

void HorizontalScrollBox::setMenu(QMenu * menu)
{
	m_menu = menu;
}

void HorizontalScrollBox::setItemMenu(QMenu * menu)
{
	m_item_menu = menu;
}

void HorizontalScrollBox::enableDragging(bool enable)
{
	if (m_drag && !enable) QDrag::cancel();
	m_dragging_enabled = enable;
}

void HorizontalScrollBox::setMIMEType(const std::string & type)
{
	m_mime_type = QString::fromStdString(type);
}

bool HorizontalScrollBox::eventFilter(QObject * o, QEvent * e)
{
	QEvent::Type type = e->type();
	// steal resize event from viewport
	if (o == m_scroll->viewport() && type == QEvent::Resize) {
		refresh();
	}

	return false;
}

QScrollArea * HorizontalScrollBox::scrollArea()
{
	return m_scroll;
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
	return std::make_pair((int)m_items.size(), -.1f);
}

void HorizontalScrollBox::refresh()
{
	m_height = m_scroll->viewport()->height();

	for (int i = 0; i < m_items.size(); i++) {
		m_items[i]->setIndex(i);
	}

	if (m_items.size() == 0) {
		m_scroll_area_widget->setMinimumWidth(0);
		return;
	}

	// do we draw a drag-n-drop box?
	int spacer_index = -1; // spacer 0 is before item 0
	if (m_drag) {
		m_drop_highlight->show();
		QPoint scrollspace = m_scroll_area_widget->mapFromGlobal(this->mapToGlobal(m_dragpos));
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

	int bheight = std::max(m_height, 10);

	for (int i = 0; i < m_items.size(); i++) {
		ScrollBoxItem *o = m_items.at(i);
		int bwidth = o->widthFromHeight(bheight);
		bwidth = std::max(bwidth, 10);

		//QRect geom(xpos, margins.top(), bwidth, bheight);
		QRect geom(xpos, 0, bwidth, bheight);
		o->setGeometry(geom);
		o->setFixedSize(bwidth, bheight);
		if (m_drag && spacer_index == i) {
			m_drop_highlight->setGeometry(xpos - m_spacing, 0, m_spacing, m_height);
		}
		xpos += bwidth;
		xpos += m_spacing;
	}
	if (m_drag && spacer_index >= m_items.size()) {
		m_drop_highlight->setGeometry(xpos - m_spacing, 0, m_spacing, m_height);
	}

	m_scroll_area_widget->setMinimumWidth(xpos);
	m_scroll_area_widget->adjustSize();
}

void HorizontalScrollBox::insertItems(const std::vector<std::pair<size_t, ScrollBoxItem*>>& insertions)
{
	for (auto &pair : insertions) {
		ScrollBoxItem *item = pair.second;
		item->setParent(m_scroll_area_widget);
		item->show();
		connect(item, &ScrollBoxItem::sMousePressEvent, this, &HorizontalScrollBox::itemMousePressEvent);
		connect(item, &ScrollBoxItem::sMouseDoubleClickEvent, this, &HorizontalScrollBox::itemMouseDoubleClickEvent);
		connect(item, &ScrollBoxItem::sMouseReleaseEvent, this, &HorizontalScrollBox::itemMouseReleaseEvent);
	}
	VecUtil::multiInsert(&m_items, insertions);
	refresh();
}

void HorizontalScrollBox::removeItems(const std::vector<size_t> indices, bool delete_items)
{	
	m_selection->clear();
	for (size_t i : indices) {
		ScrollBoxItem *item = m_items[i];
		if (delete_items) {
			delete item;
		}
	}
	VecUtil::multiRemove(&m_items, indices);
	m_selection_stack->clear();
	refresh();
}

void HorizontalScrollBox::moveItems(const std::vector<std::pair<size_t, size_t>> &mapping)
{
	VecUtil::multiMove(&m_items, mapping);
	refresh();
}

void HorizontalScrollBox::mouseMoveEvent(QMouseEvent * event)
{
	// If mouse down & beyond drag threshold then start dragging
	QPoint diff = event->globalPos() - m_mouse_down_pos;
	if (m_mime_type != ""
		&& diff.manhattanLength() > m_minimum_drag_dist
		&& m_drag == nullptr
		&& m_drag_press_index >= 0
		&& m_selection_stack->size() >= 0
		&& m_mouse_down
		&& m_dragging_enabled
		) {

		// Begin drag
		m_drag = new QDrag(this);
		ScrollBoxItem *item = getItem(m_drag_press_index);
		QPixmap pixmap(item->width(), item->height());
		item->render(&pixmap, QPoint(), QRegion(0, 0, item->width(), item->height()));
		m_drag->setPixmap(pixmap);

		QMimeData *data = new QMimeData;
		data->setData(m_mime_type, "");
		m_drag->setMimeData(data);
		m_drag->setHotSpot(event->globalPos() - item->mapToGlobal(QPoint(0, 0)));

		m_dragpos = event->pos();

		Qt::DropAction result = m_drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::MoveAction);
		m_drag = nullptr;
	}
}

void HorizontalScrollBox::mousePressEvent(QMouseEvent * event)
{
	m_selection_stack->clear();
	m_selection->clear();
	m_drag_press_index = -1;
	emit sSelectionCleared();
	if (event->button() == Qt::RightButton) {
		if (m_menu != nullptr) {
			m_menu->exec(event->globalPos());
		}
	}
	emit sTouch();
}

void HorizontalScrollBox::mouseReleaseEvent(QMouseEvent * event)
{
	m_mouse_down = false;
}

void HorizontalScrollBox::itemMousePressEvent(QMouseEvent * event, int index)
{
	if (event->button() == Qt::LeftButton) {
		//int prev = m_selection->getLastSelected();
		//if (event->type() == QEvent::MouseButtonDblClick) {
		//	//emit sDoubleClick();
		//	m_selection->add(index);
		//	return;
		//}

		if (event->modifiers() & Qt::ControlModifier) {
			// ctrl - add/remove from selection
			if (m_selection_stack->has(index)) {
				m_selection_stack->remove(index);
			}
			else {
				m_selection_stack->add(index);
			}
		}
		else if (event->modifiers() & Qt::ShiftModifier) {
			// shift - range selection
			m_selection_stack->rangeSelect(index);
		}
		else {
			// left click a selected item -> begin dragging
			m_selection_stack->selectIfNot(index);
			m_mouse_down_pos = event->globalPos();
			m_mouse_down = true;
			m_drag_press_index = index;
		}
	}
	else if (event->button() == Qt::RightButton) {
		bool cleared = m_selection_stack->selectIfNot(index);
		if (cleared) emit sSelectionCleared();

		// open menu
		if (m_item_menu) {
			m_item_menu->exec(event->globalPos());
		}
	}
	emit sTouch();
}
void HorizontalScrollBox::itemMouseReleaseEvent(QMouseEvent * event, int index)
{
	if (event->button() == Qt::LeftButton) {
		if (!(event->modifiers() & Qt::ControlModifier)
			&& !(event->modifiers() & Qt::ShiftModifier)) {
			m_selection_stack->select(index);
			emit sSelectionCleared();
		}
	}
	m_mouse_down = false;
	m_drag_press_index = -1;
}

void HorizontalScrollBox::itemMouseDoubleClickEvent(QMouseEvent * event, int index)
{
	itemMousePressEvent(event, index);
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
	if (m_drag) {
		//m_dragging = false;
		refresh();
	}
}

void HorizontalScrollBox::dragMoveEvent(QDragMoveEvent * event)
{
	if (event->mimeData()->hasFormat(m_mime_type)) {
		m_dragpos = event->pos();
		refresh();
	}
}

void HorizontalScrollBox::dropEvent(QDropEvent * event)
{
	if (event->mimeData()->hasFormat(m_mime_type) && m_drag) {
		event->accept();
		std::set<int> selection = m_selection_stack->toSet();
		std::vector<size_t> sources(selection.begin(), selection.end());
		std::vector<size_t> destinations;
		// calculate the destination position
		// m_drag_index is the spacer index
		// new position is (spacer index) - (number of items removed before it)
		size_t dest_index = m_drag_index;
		for (auto i : sources) {
			if ((int)i < m_drag_index) {
				dest_index--;
			}
		}

		for (size_t i = 0; i < sources.size(); i++) {
			destinations.push_back(dest_index + i);
		}

		// if the order hasn't changed, then don't emit a move signal
		if (sources != destinations) {
			std::vector<std::pair<size_t, size_t>> changes;
			for (size_t i = 0; i < sources.size(); i++) {
				changes.push_back(std::make_pair(sources[i], destinations[i]));
			}
			emit sMove(changes);
		}
	}
	m_drag = nullptr;
	m_drop_highlight->hide();
	emit sTouch();
}
void HorizontalScrollBox::moveTimer()
{
	if (m_drag) {
		int mx = m_dragpos.x();
		int my = m_dragpos.y();
		if (my > -20 && my < height() + 20) {
			QAbstractSlider *bar = m_scroll->horizontalScrollBar();
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
