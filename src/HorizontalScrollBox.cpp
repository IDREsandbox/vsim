#include <QScrollBar>
#include <QResizeEvent>
#include <QMouseEvent>
#include "HorizontalScrollBox.h"

HorizontalScrollBox::HorizontalScrollBox(QWidget* parent)
	: QScrollArea(parent)
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
	qDebug() << "INITAL HEIGHT?" << m_height;


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

	//// handle right-clicks for background, TODO: use a different menu
	//m_scroll_area_widget->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect(m_scroll_area_widget, &QWidget::customContextMenuRequested, this,
	//	[this](const QPoint& pos) {
	//	qDebug() << "narrative list gui - background context menu";
	//	m_slide_menu->exec(m_scroll_area_widget->mapToGlobal(pos)); }
	//);
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
	qDebug() << "height from wdith test" << m_height;
	item->widthFromHeight(m_height);
	m_items.insert(index, item);
	// steal all mouse events on items
	connect(item, &ScrollBoxItem::sMousePressEvent, this, &HorizontalScrollBox::itemMousePressEvent);
	select(index);
	item->show();
	refresh();
}

void HorizontalScrollBox::clear()
{
	for (auto ptr : m_items) {
		delete ptr;
	}
	m_items.clear();
	m_selection.clear();
	m_focus = -1;
	m_last_selected = -1;

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

void HorizontalScrollBox::addToSelection(int index)
{
	m_selection.insert(index);
	m_items[index]->colorSelect(true);
}
void HorizontalScrollBox::removeFromSelection(int index) 
{
	m_selection.erase(index);
	m_items[index]->colorSelect(false);
}

void HorizontalScrollBox::select(int index)
{
	clearSelection();
	m_selection.insert(index);
	m_items[index]->colorSelect(true);
}

bool HorizontalScrollBox::isSelected(int index)
{
	auto it = m_selection.find(index);
	return it != m_selection.end(); // if end then not selected
}

void HorizontalScrollBox::deleteItem(int position)
{
	clearSelection();
	ScrollBoxItem* item = m_items.takeAt(position);
	delete item;
	refresh();
}

void HorizontalScrollBox::openMenu(QPoint globalPos)
{
}

void HorizontalScrollBox::openItemMenu(QPoint globalPos)
{
}

void HorizontalScrollBox::setSpacing(int space)
{
	m_spacing = space;
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
	qDebug() << event->angleDelta();
	horizontalScrollBar()->setValue(horizontalScrollBar()->value() - event->delta());
}

void HorizontalScrollBox::mousePressEvent(QMouseEvent * event)
{
	qDebug() << "scroll area mouse event\n";
	clearSelection();
	if (event->button() == Qt::RightButton) {
		qDebug() << "open area mouse press";
		openMenu(event->globalPos());
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
		openItemMenu(event->globalPos());
		refresh();
	}
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

