#include <QScrollBar>
#include <QResizeEvent>
#include "HorizontalScrollBox.h"


HorizontalScrollBox::HorizontalScrollBox(QWidget* parent) : QScrollArea(parent)
{
	this->setObjectName(QStringLiteral("scrollArea"));
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setWidgetResizable(true);

	m_scroll_area_widget = new QWidget(this);
	m_scroll_area_widget->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
	m_scroll_area_widget->setGeometry(QRect(0, 0, 653, 316));
	m_scroll_area_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	m_scroll_area_widget->setStyleSheet(QStringLiteral("background-color: rgb(200, 100, 200);"));
	this->setWidget(m_scroll_area_widget);

	//m_layout = new CardLayout(m_scroll_area_widget, 10);
	//m_layout->setHeight(m_scroll_area_widget->height());
	//m_layout->setContentsMargins(0, 0, 0, 0);
	
	addItem("FOO THE BAR");
	addItem("BAR The foool?");
	insertItem(1, "Insert me at 1 (aka 2)");

	for (int i = 0; i < 100; i++) {
		addItem("some itemmm");
	}

	insertItem(0, "the first of course");
	addItem("the LAST ITEM!");
}

void HorizontalScrollBox::addItem(QString text)
{
	insertItem(m_items.size(), text);
	horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
}

void HorizontalScrollBox::insertItem(int index, QString text)
{
	auto newWidget = new ScrollBoxItem(m_scroll_area_widget);
	newWidget->setText(QString::number(index + 1) + QString(" - ") + text);

	newWidget->setStyleSheet(QString("background-color: rgb(") + QString::number((index * 40) % 255) + "," + QString::number(((index % 7) * 50) % 255) + "," + QString::number(100) + ");");

	m_items.insert(index, newWidget);
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

	// minimum width
	int newBoxWidth = floor(m_height*m_ratio);
	int newBoxHeight = m_height;

	int totalspace = m_spacing*(qMax(0, m_items.size() - 1));

	//int minwidth = margins.left() + margins.right() + totalspace + list.size() * boxWidth();
	int minwidth = totalspace + m_items.size() * newBoxWidth;
	m_scroll_area_widget->setMinimumWidth(minwidth);

	positionChildren();

	// deal with other stuff, like the scrollbar and whatever
	QScrollArea::resizeEvent(event);

	int newSegmentSize = newBoxWidth + m_spacing;

	int newValue;
	if (nomargin <= 0) {
		newValue = oldValue;
	}
	else {
		newValue = leftmargin + segmentNumber * newSegmentSize + segmentOffset;//((framePortion > 1.0) ? newBoxWidth + 
	}
	
	bar->setValue(newValue);
}
