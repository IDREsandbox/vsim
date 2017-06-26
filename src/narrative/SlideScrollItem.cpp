#include <QDebug>
#include "SlideScrollItem.h"
#include "SlideScrollWidget.h"

SlideScrollItem::SlideScrollItem() 
	: ScrollBoxItem()
{
	//m_number_label = new QLabel("??", this);
	//m_number_label->setStyleSheet(QLatin1String("color: rgb(255, 255, 255);\n"
	//	"font: 12pt;"));

	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(m_layout);

	
	// transition widget
	m_trans_widget = new SlideTransitionScrollWidget(this);
	m_layout->addWidget(m_trans_widget);
	m_trans_widget->setStyleSheet("background-color: rgb(40, 40, 40);");

	// trans widget size policy
	QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
	spRight.setHorizontalStretch(3);
	m_trans_widget->setSizePolicy(spRight);


	// slide widget
	m_slide_widget = new SlideScrollWidget(this);
	m_layout->addWidget(m_slide_widget);

	// slide widget size policy
	QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
	spLeft.setHorizontalStretch(16);
	m_slide_widget->setSizePolicy(spLeft);

}

void SlideScrollItem::setIndex(int index)
{
	if (index == 0) {
		m_trans_widget->hide();
	}
	//m_slide_widget->setText(QString::number(index));
	m_slide_widget->setNumber(index+1);
	ScrollBoxItem::setIndex(index);
}

int SlideScrollItem::widthFromHeight(int height)
{
	if (getIndex() == 0) {
		return (16.0 / 9.0)*height;
	}
	return (19.0/9.0)*height;
}

SlideScrollWidget * SlideScrollItem::getSlideWidget()
{
	return m_slide_widget;
}

SlideTransitionScrollWidget * SlideScrollItem::getTransitionWidget()
{
	return m_trans_widget;
}

void SlideScrollItem::mousePressEvent(QMouseEvent * event)
{
	qDebug() << "zz slide mouse press event" << event;
}
