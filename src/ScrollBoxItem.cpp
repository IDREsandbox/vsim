#include "ScrollBoxItem.h"

#include <QPainter>
#include <QPaintEvent>

ScrollBoxItem::ScrollBoxItem(QWidget *parent)
	: QFrame(parent),
	m_selected(false)
{
	m_select_color = QColor(0, 100, 255);
	m_color = QColor(60, 60, 60);
}

void ScrollBoxItem::setIndex(int index)
{
	m_index = index;
}

int ScrollBoxItem::getIndex() const
{
	return m_index;
}

void ScrollBoxItem::select(bool s)
{
	if (m_selected == s) return;
	m_selected = s;
	update();
}

int ScrollBoxItem::widthFromHeight(int height) const
{
	return height;
}

bool ScrollBoxItem::event(QEvent * e)
{
	return QWidget::event(e);
}
void ScrollBoxItem::paintEvent(QPaintEvent * e)
{
	QPainter p(this);

	QColor c = m_selected ? m_select_color : m_color;
	p.setBrush(QBrush(QColor(c)));
	p.drawRect(0, 0, width(), height());
}

void ScrollBoxItem::mousePressEvent(QMouseEvent * event)
{
	emit sMousePressEvent(event, m_index);
}

void ScrollBoxItem::mouseDoubleClickEvent(QMouseEvent * event)
{
	emit sMouseDoubleClickEvent(event, m_index);
}

void ScrollBoxItem::mouseReleaseEvent(QMouseEvent * event)
{
	emit sMouseReleaseEvent(event, m_index);
}
