#include <QDebug>
#include "SlideTransitionScrollItem.h"

SlideTransitionScrollItem::SlideTransitionScrollItem()
	: ScrollBoxItem() {

	m_duration_label = new QLabel("3.5", this);
	m_duration_label->setStyleSheet(QLatin1String("color: rgb(255, 255, 255);\n"
		"font: 8pt;"));
	m_duration_label->setAlignment(Qt::AlignCenter);
	//m_duration_label->setContentsMargins(0,0,0,0);

	// center middle
	m_layout = new QVBoxLayout(this);
	setLayout(m_layout);
	m_layout->addWidget(m_duration_label);
	m_layout->setContentsMargins(1, 0, 1, 0);
}

int SlideTransitionScrollItem::widthFromHeight(int height)
{
	return (1.0/3.0)*height;
}
