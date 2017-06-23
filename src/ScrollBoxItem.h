#ifndef SCROLLBOXITEM_H
#define SCROLLBOXITEM_H

#include <QFrame>
#include <QWidget>
#include <QMouseEvent>
#include <QDebug>

class Foo : QFrame {
	Q_OBJECT
public:

	void foo();
};


class ScrollBoxItem : public QFrame {
	Q_OBJECT
public:
	ScrollBoxItem(QWidget* parent);
	
	// interface for controller
	
	// interface for gui
	// index is used for pretty numbering
	
	// TODO: fix naming style
	void setIndex(int index);
	int getIndex(int index);
	void colorFocus(bool color);
	void colorSelect(bool color);
signals:
	void sMousePressEvent(QMouseEvent *event, int index);
	
protected:
	void mousePressEvent(QMouseEvent *event);
		
private:
	int m_index = -1;
};


#endif