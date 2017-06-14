#include <QScrollBar>
#include <QResizeEvent>
#include "HorizontalScrollBox.h"

#include <QCoreApplication>

HorizontalScrollBox::HorizontalScrollBox(QWidget* parent)
	:	QScrollArea(parent),
		m_selection(NULL)
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

	// handle right-clicks on items
	newWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(newWidget, &QWidget::customContextMenuRequested, this, 
		[this, newWidget](const QPoint& pos) { 
			this->onItemMenu(pos, newWidget); 
		}
	);

	select(newWidget);
	newWidget->show();
	setWidgetWidth();
}

void HorizontalScrollBox::deleteSelection()
{
	int index = getIndexOf(m_selection);
	if (index == -1) return;
	deleteItem(index);
}

void HorizontalScrollBox::select(QWidget* widget)
{
	if (m_selection != NULL) {
		//widget->unhighlight
		m_selection->setStyleSheet("background-color: rgb(100, 100, 100);");
	}
	// TODO: guarantee existence
	m_selection = widget;
	//widget->highlight
	widget->setStyleSheet("background-color: rgb(0, 100, 255);");
}

void HorizontalScrollBox::deselect()
{
	if (m_selection != NULL) {
		// widget->unhighlight
		m_selection->setStyleSheet("background-color: rgb(100, 100, 100);");
	}
	m_selection = NULL;
}

void HorizontalScrollBox::deleteItem(int position)
{
	deselect();
	QWidget* item = m_items.takeAt(position);
	delete item;
	if (m_items.length() > 0) {
		select(m_items.last()); // TODO: replace with something else
	}
	setWidgetWidth();
}

void HorizontalScrollBox::onItemMenu(QPoint localPos, ScrollBoxItem* widget)
{
	select(widget);
	m_slide_menu->exec(widget->mapToGlobal(localPos));
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

int HorizontalScrollBox::getIndexOf(QWidget* widget)
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
		QWidget *o = m_items.at(i);

		//QRect geom(xpos, margins.top(), bwidth, bheight);
		QRect geom(xpos, 0, bwidth, bheight);
		o->setGeometry(geom);

		xpos += bwidth;
		xpos += m_spacing;
	}
}