#include <QDebug>
#include "SlideScrollItem.h"

SlideScrollItem::SlideScrollItem() 
	: ScrollBoxItem()
{
	m_number_label = new QLabel("??", this);
	m_number_label->setStyleSheet(QLatin1String("color: rgb(255, 255, 255);\n"
		"font: 12pt;"));
}

