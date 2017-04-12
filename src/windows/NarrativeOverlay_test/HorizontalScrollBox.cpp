#include "HorizontalScrollBox.h"
#include "NarrativeLayout.h"

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

	m_layout = new CardLayout(m_scroll_area_widget, 10);
	m_layout->setHeight(m_scroll_area_widget->height());
	m_layout->setContentsMargins(10, 10, 10, 10);
	auto cm = m_layout->contentsMargins();

	for (int i = 0; i < 100; i++) {
		//auto wx = new QWidget(saw);
		auto wx = new QLabel(QString::number(i), m_scroll_area_widget);
		wx->setStyleSheet(QString("background-color: rgb(") + QString::number((i * 40) % 255) + "," + QString::number(((i % 7) * 50) % 255) + "," + QString::number(100) + ");");
		//wx->setStyleSheet(QString("background-color: rgb(") + QString::number(100) + "," + QString::number(0) + "," + QString::number(0) + ");");
		wx->setGeometry(0, 0, 100, 100);
		//wx->setFixedWidth(100);
		//wx->setFixedHeight(300);
		//wx->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
		m_layout->addWidget(wx);
	}
}

void HorizontalScrollBox::resizeEvent(QResizeEvent* event)
{
	//m_scroll_area_widget->setMinimumWidth(40000);
	//m_scroll_area_widget->setMinimumSize(40000, 100);

	// snapshot of current position and all that
	int oldValue = horizontalScrollBar()->value();
	auto bar = horizontalScrollBar();

	// we want to keep the same frame on the left side when we resize
	int leftmargin = m_layout->contentsMargins().left();
	int boxWidth = m_layout->boxWidth();
	//int leftmargin = 10;
	//int boxWidth = height();

	int nomargin = oldValue - leftmargin;
	int segmentSize = boxWidth + m_layout->spacing();
	int segmentNumber = nomargin / (segmentSize); // [    frame    ][space] <- one segment
	int segmentOffset = nomargin % segmentSize;
	//float framePortion = std::min(1.0f, segmentOffset / (float)boxWidth); // 0-1 if in the frame, 1 if in the spacer
	//int spacerOffset = std::max(0, segmentOffset - boxWidth); // 0 if in the frame, 0-space is in the spacer
	
	//int frameOffest = // floating point is too much of a pain, just use the pixel #
	
	// tell layout the new height
	m_layout->setHeight(event->size().height());

	// get the minimum width
	int widgetMinWidth = m_layout->getMinWidth();

	// set the widget's width
	auto size = m_scroll_area_widget->size();
	size.setWidth(widgetMinWidth);
	m_scroll_area_widget->setMinimumWidth(widgetMinWidth);


	// deal with other stuff, like the scrollbar and whatever
	QScrollArea::resizeEvent(event);


	int newBoxWidth = m_layout->boxWidth();
	//int newBoxWidth = height();
	int newSegmentSize = newBoxWidth + m_layout->spacing(); // circular references back to our height... bad practice

	int newValue;
	if (nomargin <= 0) {
		newValue = oldValue;
	}
	else {
		newValue = leftmargin + segmentNumber * newSegmentSize + segmentOffset;//((framePortion > 1.0) ? newBoxWidth + 
	}
	
	bar->setValue(newValue);
}
