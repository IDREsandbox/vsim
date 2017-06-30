#include "ScrollBoxItem.h"

ScrollBoxItem::ScrollBoxItem()
	: QFrame(nullptr)
{
}

void ScrollBoxItem::setIndex(int index)
{
	m_index = index;
}

int ScrollBoxItem::getIndex()
{
	return m_index;
}

void ScrollBoxItem::colorFocus(bool color)
{
	setStyleSheet("background-color: rgb(0, 100, 255);");
	// do something else? like draw a rectangle
}

void ScrollBoxItem::colorSelect(bool color)
{
	if (color) {
		setStyleSheet("background-color: rgb(0, 100, 255);");
	}
	else {
		setStyleSheet("background-color: rgb(70,70,70);");
	}
}

int ScrollBoxItem::widthFromHeight(int height)
{
	return (16.0f / 9.0f) * height;
}

void ScrollBoxItem::mousePressEvent(QMouseEvent *event)
{
	qDebug() << "ScrollBoxItem mouse press event " << event->type() << m_index; // << this->text();
	emit sMousePressEvent(event, m_index);
}

void ScrollBoxItem::mouseReleaseEvent(QMouseEvent * event)
{
	qDebug() << "ScrollBoxItem mouse release event ScrollBoxItem" << event->type() << m_index; // << this->text();
	emit sMouseReleaseEvent(event, m_index);
}
