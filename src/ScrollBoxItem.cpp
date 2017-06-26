#include "ScrollBoxItem.h"

ScrollBoxItem::ScrollBoxItem()
	: QFrame(nullptr)
{
}

void ScrollBoxItem::setIndex(int index)
{
	m_index = index;
	//setText(QString::number(index));
}

int ScrollBoxItem::getIndex(int index)
{
	return m_index;
}

void ScrollBoxItem::colorFocus(bool color)
{
	//setStyleSheet("background-color: rgb(0, 100, 255);");
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
	qDebug() << "mouse event ScrollBoxItem" << event->type() << m_index; // << this->text();
	emit sMousePressEvent(event, m_index);
}
