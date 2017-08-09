#include <QScrollBar>
#include <QResizeEvent>
#include <QMouseEvent>
#include "HorizontalScrollBox.h"

HorizontalScrollBox::HorizontalScrollBox(QWidget* parent)
	: QScrollArea(parent),
	m_menu(nullptr),
	m_item_menu(nullptr),
	m_group(nullptr)
{
	this->setObjectName(QStringLiteral("scrollArea"));
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setWidgetResizable(true);

	m_scroll_area_widget = new QWidget(this);
	m_scroll_area_widget->setStyleSheet(
		"color: rgb(255, 255, 255);"
		//"QMenu {"
		//"	background-color: rgb(255,255,255);"
		//"}"
	);
	this->setWidget(m_scroll_area_widget);
	
	//m_height = m_scroll_area_widget->height();
	m_height = this->height();

	const char *scrollbar_style =
		"QScrollBar:horizontal {		  "
		"    border: 2px solid grey;	  "
		"    background: #112244;		  "
		"    height: 15px;				  "
		"    margin: 0px 20px 0 20px;	  "
		"}								  "
		"QScrollBar::handle:horizontal {  "
		"    background: #666666;			  "
		"    min-width: 20px;			  "
		"}								  "
		"QScrollBar::add-line:horizontal {"
		"    border: 2px solid grey;	  "
		"    background: #112244;		  "
		"    width: 20px;				  "
		"    subcontrol-position: right;  "
		"    subcontrol-origin: margin;	  "
		"}								  "
		"								  "
		"QScrollBar::sub-line:horizontal {"
		"    border: 2px solid grey;	  "
		"    background: #112244;		  "
		"    width: 20px;				  "
		"    subcontrol-position: left;	  "
		"    subcontrol-origin: margin;	  "
		"}								  "
		"QScrollBar::add-page:horizontal, "
		"QScrollBar::sub-page:horizontal {"
		"	background: none;"
		"}"
		;

	this->horizontalScrollBar()->setStyleSheet(scrollbar_style);

}

void HorizontalScrollBox::addItem(ScrollBoxItem *item)
{
	insertItem(m_items.size(), item);
	horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
}

void HorizontalScrollBox::insertItem(int index, ScrollBoxItem *item)
{
	
	qDebug() << "narrative list gui - insert item at" << index;
	item->setParent(m_scroll_area_widget);
	// approximate geometry, will be fixed later on in refresh()
	item->setGeometry(0, 0, m_height, 2.5*m_height);
	item->widthFromHeight(m_height);
	m_items.insert(index, item);
	// steal all mouse events on items, alternatively we could use a filter
	connect(item, &ScrollBoxItem::sMousePressEvent, this, &HorizontalScrollBox::itemMousePressEvent);
	select(index);
	item->show();
	refresh();
	m_last_selected = index;
}

void HorizontalScrollBox::insertNewItem(int index)
{
	osg::Node *node = nullptr;
	if (m_group != nullptr && index <= m_group->getNumChildren()) {
		node = m_group->getChild(index);
		qDebug() << "getting child" << index << node;
	}

	ScrollBoxItem *item = createItem(node); // create the item, virtual
	if (item == nullptr) return;
	HorizontalScrollBox::insertItem(index, item);
}

void HorizontalScrollBox::clear()
{
	qDebug() << "clearing";

	for (auto ptr : m_items) {
		delete ptr;
	}
	m_items.clear();
	m_selection.clear();
	m_last_selected = -1;

	if (m_group) disconnect(m_group, 0, this, 0);
	m_group = nullptr;

	refresh();
}

void HorizontalScrollBox::deleteSelection()
{
	QList<ScrollBoxItem*> newlist;
	// null out the deleted ones
	for (auto i : m_selection){
		delete m_items[i];
		m_items[i] = NULL;
	}
	// copy over
	for (int i = 0; i < m_items.size(); i++) {
		auto x = m_items.at(i);
		if (x != NULL) newlist.append(x);
	}

	m_items = newlist;
	m_selection.clear();
	refresh();
}

void HorizontalScrollBox::clearSelection()
{
	for (auto i : m_items) {
		i->colorSelect(false);
	}
	m_selection.clear();
	emit sSelectionChange();
}

void HorizontalScrollBox::setSelection(std::set<int> selection)
{
	clearSelection();
	for (auto i : selection) {
		if (i < 0 || i >= m_items.size()) continue;
		addToSelection(i);
	}

	// have the viewport focus the first selected item
	if (!selection.empty()) {
		int first = *selection.begin();
		if (first < 0) return;
		if (first >= m_items.size()) return;
		ensureWidgetVisible(m_items[first]);
	}
}

const std::set<int>& HorizontalScrollBox::getSelection()
{
	return m_selection;
}

int HorizontalScrollBox::getLastSelected()
{
	return m_last_selected;
}

ScrollBoxItem *HorizontalScrollBox::getItem(int position)
{
	if (position < m_items.size()) {
		return m_items.at(position);
	}
	return nullptr;
}

// simple code given an abstract selection thing
// on selection change (index, bool)
// m_items[index]->colorSelect(bool);

void HorizontalScrollBox::addToSelection(int index)
{
	m_selection.insert(index);
	m_items[index]->colorSelect(true);
	emit sSelectionChange();
}
void HorizontalScrollBox::removeFromSelection(int index) 
{
	m_selection.erase(index);
	m_items[index]->colorSelect(false);
	emit sSelectionChange();
}

void HorizontalScrollBox::select(int index)
{
	clearSelection();
	m_selection.insert(index);
	m_items[index]->colorSelect(true);
	emit sSelectionChange();
}

bool HorizontalScrollBox::isSelected(int index)
{
	auto it = m_selection.find(index);
	return it != m_selection.end(); // if end then not selected
}

void HorizontalScrollBox::deleteItem(int position)
{
	m_last_selected = -1;
	clearSelection();
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

	for (uint i = 0; i < group->getNumChildren(); i++) {
		insertNewItem(i);
	}
}

void HorizontalScrollBox::setSpacing(int space)
{
	m_spacing = space;
	refresh();
}

void HorizontalScrollBox::forceSelect(int index)
{
	blockSignals(true);
	select(index);
	blockSignals(false);
}

ScrollBoxItem * HorizontalScrollBox::createItem(osg::Node *)
{
	return nullptr;
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
	qDebug() << event->angleDelta();
	horizontalScrollBar()->setValue(horizontalScrollBar()->value() - event->delta());
}

void HorizontalScrollBox::mousePressEvent(QMouseEvent * event)
{
	qDebug() << "scroll area mouse event\n";
	clearSelection();
	if (event->button() == Qt::RightButton) {
		qDebug() << "open area right click";
		if (m_menu != nullptr) {
			m_menu->exec(event->globalPos());
		}
	}
}

void HorizontalScrollBox::itemMousePressEvent(QMouseEvent * event, int index)
{
	if (event->button() == Qt::LeftButton) {
		qDebug() << "item press - lmb";
		m_last_selected = index;
		if (event->type() == QEvent::MouseButtonDblClick) {
			qDebug() << "item press - double click";
			//emit sDoubleClick();
			addToSelection(index);
			refresh();
			return;
		}

		if (event->modifiers() & Qt::ControlModifier) {
			if (isSelected(index)) {
				removeFromSelection(index);
			}
			else {
				addToSelection(index);
			}
			qDebug() << "item press - shift";
		}
		else {
			select(index);
		}
		refresh();
	}
	else if (event->button() == Qt::RightButton) {
		qDebug() << "item press - rmb";
		m_last_selected = index;
		if (!isSelected(index)) {
			select(index);
		}
		// open menu
		if (m_item_menu) m_item_menu->exec(event->globalPos());
		refresh();
	}
}

void HorizontalScrollBox::setLastSelected(int idx) {
	m_last_selected = idx;
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

	int xpos = m_spacing;

	int bheight = m_height;

	for (int i = 0; i < m_items.size(); i++) {
		ScrollBoxItem *o = m_items.at(i);
		int bwidth = o->widthFromHeight(bheight);

		//QRect geom(xpos, margins.top(), bwidth, bheight);
		QRect geom(xpos, 0, bwidth, bheight);
		o->setGeometry(geom);

		xpos += bwidth;
		xpos += m_spacing;
	}

	m_scroll_area_widget->setMinimumWidth(xpos);
}

