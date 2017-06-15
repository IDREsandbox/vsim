#include <QScrollBar>
#include <QResizeEvent>
#include <QMouseEvent>
#include "HorizontalScrollBox.h"

HorizontalScrollBox::HorizontalScrollBox(QWidget* parent)
	:	QScrollArea(parent)
{
	this->setObjectName(QStringLiteral("scrollArea"));
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setWidgetResizable(true);

	m_scroll_area_widget = new QWidget(this);
	m_scroll_area_widget->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
	//m_scroll_area_widget->setGeometry(QRect(0, 0, 653, 316));
	//m_scroll_area_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	//m_scroll_area_widget->setStyleSheet(QStringLiteral("background-color: rgb(200, 100, 200);"));

	// Sam - is this right?
	const char *style =
		"QScrollBar:horizontal {		  "
		"    border: 2px solid grey;	  "
		"    background: #32CC99;		  "
		"    height: 15px;				  "
		"    margin: 0px 20px 0 20px;	  "
		"}								  "
		"QScrollBar::handle:horizontal {  "
		"    background: white;			  "
		"    min-width: 20px;			  "
		"}								  "
		"QScrollBar::add-line:horizontal {"
		"    border: 2px solid grey;	  "
		"    background: #32CC99;		  "
		"    width: 20px;				  "
		"    subcontrol-position: right;  "
		"    subcontrol-origin: margin;	  "
		"}								  "
		"								  "
		"QScrollBar::sub-line:horizontal {"
		"    border: 2px solid grey;	  "
		"    background: #32CC99;		  "
		"    width: 20px;				  "
		"    subcontrol-position: left;	  "
		"    subcontrol-origin: margin;	  "
		"}								  "
		"QScrollBar::add-page:horizontal, "
		"QScrollBar::sub-page:horizontal {"
		"	background: none;"
		"}"
		;

	this->horizontalScrollBar()->setStyleSheet(style);

	for (int i = 0; i < 10; i++) {
		this->addItem("foo");
	}

	this->setWidget(m_scroll_area_widget);

	// initialize menus
	m_slide_menu = new QMenu(tr("Slide context menu"), this);
	//QAction actionCut("Cut", m_slide_menu);
	//QAction actionCopy("Copy", m_slide_menu);
	//QAction actionPaste("Paste", m_slide_menu);
	QAction* actionNew = new QAction("New Slide", m_slide_menu);
	QAction* actionDelete = new QAction("Delete Slide", m_slide_menu);
	// temporary direct connection
	connect(actionNew, &QAction::triggered, this, &HorizontalScrollBox::addBlankItem);
	connect(actionDelete, &QAction::triggered, this, &HorizontalScrollBox::deleteSelection);
	m_slide_menu->addAction(actionNew);
	m_slide_menu->addAction(actionDelete);

	// handle right-clicks for background, TODO: use a different menu
	m_scroll_area_widget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_scroll_area_widget, &QWidget::customContextMenuRequested, this,
		[this](const QPoint& pos) {
		qDebug() << "background context menu";
		m_slide_menu->exec(m_scroll_area_widget->mapToGlobal(pos)); }
	);
}

void HorizontalScrollBox::addBlankItem()
{
	// TODO: add after selection
	addItem("A BRAND NEW ITEM!");
}

void HorizontalScrollBox::addItem(QString text)
{
	insertItem(m_items.size(), text);
	horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
}

void HorizontalScrollBox::insertItem(int index, QString text)
{
	qDebug() << "insert item";
	auto newWidget = new ScrollBoxItem(m_scroll_area_widget);
	newWidget->setText(QString::number(index + 1) + QString(" - ") + text);
	newWidget->setStyleSheet(QString("background-color: rgb(") + QString::number((index * 40) % 255) + "," + QString::number(((index % 7) * 50) % 255) + "," + QString::number(100) + ");");

	m_items.insert(index, newWidget);

	// steal all mouse events on items
	connect(newWidget, &ScrollBoxItem::sMousePressEvent, this, &HorizontalScrollBox::itemMousePressEvent);

	select(index);

	//select(newWidget);
	newWidget->show();
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
		i->ColorSelect(false);
	}
	m_selection.clear();
}

std::set<int> HorizontalScrollBox::getSelection()
{
	return m_selection;
}

int HorizontalScrollBox::getLastSelected()
{
	return m_last_selected;
}

ScrollBoxItem * HorizontalScrollBox::getItem(int position)
{
	if (position < m_items.size()) {
		return m_items.at(position);
	}
	return nullptr;
}

void HorizontalScrollBox::addToSelection(int index)
{
	m_selection.insert(index);
	m_items[index]->ColorSelect(true);
}
void HorizontalScrollBox::removeFromSelection(int index) 
{
	m_selection.erase(index);
	m_items[index]->ColorSelect(false);
}

void HorizontalScrollBox::select(int index)
{
	clearSelection();
	m_selection.insert(index);
	m_items[index]->ColorSelect(true);
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
	setWidgetWidth();
}

void HorizontalScrollBox::openMenu(QPoint globalPos)
{
	m_slide_menu->exec(globalPos);
}

void HorizontalScrollBox::resizeEvent(QResizeEvent* event)
{
	// snapshot of current position and all that
	int oldValue = horizontalScrollBar()->value();
	auto bar = horizontalScrollBar();

	// we want to keep the same frame on the left side when we resize
	int leftmargin = 0;//m_layout->contentsMargins().left();
	int boxWidth = floor(m_height*m_ratio); //int boxWidth = m_layout->boxWidth();

	int nomargin = oldValue - leftmargin;
	int segmentSize = boxWidth + m_spacing;
	int segmentNumber = nomargin / (segmentSize); // [    frame    ][space] <- one segment
	int segmentOffset = nomargin % segmentSize;
	//float framePortion = std::min(1.0f, segmentOffset / (float)boxWidth); // 0-1 if in the frame, 1 if in the spacer
	//int spacerOffset = std::max(0, segmentOffset - boxWidth); // 0 if in the frame, 0-space is in the spacer
	
	// tell layout the new height
	//m_layout->setHeight(event->size().height());
	m_height = event->size().height();
	setWidgetWidth();

	// minimum width
	int newBoxWidth = floor(m_height*m_ratio);
	int newBoxHeight = m_height;

	// deal with other stuff
	QScrollArea::resizeEvent(event);

	int newSegmentSize = newBoxWidth + m_spacing;

	// reposition the scrollbar so that it's over the original slide
	int newValue;
	if (nomargin <= 0) {
		newValue = oldValue;
	}
	else {
		newValue = leftmargin + segmentNumber * newSegmentSize + segmentOffset;//((framePortion > 1.0) ? newBoxWidth + 
	}
	
	bar->setValue(newValue);
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
}

void HorizontalScrollBox::itemMousePressEvent(QMouseEvent * event, int index)
{
	qDebug() << "item mouse presse event " << index;
	if (event->button() == Qt::LeftButton) {
		qDebug() << "lmb";
		m_last_selected = index;
		if (event->modifiers() & Qt::ShiftModifier) {
			if (isSelected(index)) {
				removeFromSelection(index);
			}
			else {
				addToSelection(index);
			}
			qDebug() << "shift";
		}
		else {
			select(index);
		}
		refresh();
	}
	else if (event->button() == Qt::RightButton) {
		qDebug() << "rmb";
		m_last_selected = index;
		if (!isSelected(index)) {
			select(index);
		}
		openMenu(event->globalPos());
		refresh();
	}
}

void HorizontalScrollBox::refresh()
{
	for (int i = 0; i < m_items.size(); i++) {
		m_items[i]->SetIndex(i);
	}
	setWidgetWidth();
}

void HorizontalScrollBox::setWidgetWidth()
{
	// minimum width
	int newBoxWidth = floor(m_height*m_ratio);
	int newBoxHeight = m_height;

	int totalspace = m_spacing*(qMax(0, m_items.size() - 1));

	//int minwidth = margins.left() + margins.right() + totalspace + list.size() * boxWidth();
	int minwidth = totalspace + m_items.size() * newBoxWidth;
	m_scroll_area_widget->setMinimumWidth(minwidth);

	positionChildren();
}

int HorizontalScrollBox::getIndexOf(ScrollBoxItem* widget)
{
	return m_items.indexOf(widget);
}

void HorizontalScrollBox::positionChildren()
{
	//auto margins = contentsMargins();
	//QMargins margins = QMargins(5, 5, 5, 5);

	if (m_items.size() == 0)
		return;

	int xpos = 0;
	//xpos += margins.left();

	//m_height = r.height()
	int bheight = m_height;
	int bwidth = m_height*m_ratio;

	for (int i = 0; i < m_items.size(); i++) {
		ScrollBoxItem *o = m_items.at(i);

		//QRect geom(xpos, margins.top(), bwidth, bheight);
		QRect geom(xpos, 0, bwidth, bheight);
		o->setGeometry(geom);

		xpos += bwidth;
		xpos += m_spacing;
	}
}

