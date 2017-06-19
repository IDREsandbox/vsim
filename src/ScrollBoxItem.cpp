#include "ScrollBoxItem.h"


ScrollBoxItem::ScrollBoxItem(QWidget* parent)
	: QLabel(parent)
{
}

int ScrollBoxItem::foo()
{
	return 0;
}

void ScrollBoxItem::SetIndex(int index)
{
	m_index = index;
	setText(QString::number(index));
}

int ScrollBoxItem::GetIndex(int index)
{
	return m_index;
}

void ScrollBoxItem::ColorFocus(bool color)
{
	//setStyleSheet("background-color: rgb(0, 100, 255);");
	// do something else? like draw a rectangle
}

void ScrollBoxItem::ColorSelect(bool color)
{
	if (color) {
		setStyleSheet("background-color: rgb(0, 100, 255);");
	}
	else {
		setStyleSheet("background-color: rgb(100,100,100);");
	}
}

void ScrollBoxItem::mousePressEvent(QMouseEvent * event)
{
	qDebug() << "mouse event" << this->text();
	emit sMousePressEvent(event, m_index);
}
