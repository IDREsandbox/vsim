#include "SlideTransitionScrollWidget.h"

SlideTransitionScrollWidget::SlideTransitionScrollWidget(QWidget * parent) : QWidget(parent) {

	// trans widget stuff
	m_layout = new QVBoxLayout(this);
	setLayout(m_layout);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_duration_label = new QLabel("1.1s", this);
	m_layout->addWidget(m_duration_label);
	m_duration_label->setAlignment(Qt::AlignCenter);
}

void SlideTransitionScrollWidget::setDuration(float t)
{
	m_duration_label->setText(QString::number(t) + "s");
}

void SlideTransitionScrollWidget::mousePressEvent(QMouseEvent *event)
{
	emit sMousePressEvent(event);
}
