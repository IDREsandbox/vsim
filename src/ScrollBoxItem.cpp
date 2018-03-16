#include "ScrollBoxItem.h"

ScrollBoxItem::ScrollBoxItem(QWidget *parent)
	: QFrame(parent),
	m_selected(false)
{
	setDeselectStyle("ScrollBoxItem { background: rgb(48, 48, 48, 200); }");
	setSelectStyle("ScrollBoxItem { background: rgb(0, 100, 255); }");
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
	if (s) {
		setStyleSheet(m_select_style);
	}
	else {
		setStyleSheet(m_style);
	}
}

int ScrollBoxItem::widthFromHeight(int height) const
{
	return height;
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

void ScrollBoxItem::setDeselectStyle(QString style)
{
	m_style = style;
	if (!m_selected) {
		setStyleSheet(style);
	}
}

void ScrollBoxItem::setSelectStyle(QString style)
{
	m_select_style = style;
	if (m_selected) {
		setStyleSheet(style);
	}
}
