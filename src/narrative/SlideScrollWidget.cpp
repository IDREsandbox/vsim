#include "SlideScrollWidget.h"

SlideScrollWidget::SlideScrollWidget(QWidget * parent) : QWidget(parent) {
	m_number_label = new QLabel("foo", this);
}

void SlideScrollWidget::setNumber(int n)
{
	m_number_label->setText(QString::number(n));
}

void SlideScrollWidget::mousePressEvent(QMouseEvent *event)
{
	emit sMousePressEvent(event);
}