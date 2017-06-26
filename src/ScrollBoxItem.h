#ifndef SCROLLBOXITEM_H
#define SCROLLBOXITEM_H

#include <QFrame>
#include <QWidget>
#include <QMouseEvent>
#include <QDebug>

class ScrollBoxItem : public QFrame {
	Q_OBJECT
public:
	ScrollBoxItem();
	
	// interface for controller is in derived classes
	
	// interface for gui
	// index is used for pretty numbering
	void setIndex(int index);
	int getIndex(int index);
	void colorFocus(bool color);
	void colorSelect(bool color);

	virtual int widthFromHeight(int height);

signals:
	void sMousePressEvent(QMouseEvent *event, int index);
	
protected:
	void mousePressEvent(QMouseEvent *event);
		
private:
	int m_index = -1;
};


#endif