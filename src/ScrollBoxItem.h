#ifndef SCROLLBOXITEM_H
#define SCROLLBOXITEM_H

#include <QtWidgets/QLabel>
#include <QMouseEvent>
#include <QDebug>

class ScrollBoxItem : public QFrame {
	Q_OBJECT
public:
	ScrollBoxItem(QWidget* parent);

	// interface for controller

	// interface for gui
	// index is used for pretty numbering

	// TODO: fix naming style
	void SetIndex(int index);
	int GetIndex(int index);
	void ColorFocus(bool color);
	void ColorSelect(bool color);
signals:
	void sMousePressEvent(QMouseEvent* event, int index);

protected:
	void mousePressEvent(QMouseEvent* event);
private:
	int m_index = -1;
};

#endif