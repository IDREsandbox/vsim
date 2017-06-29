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
	virtual void setIndex(int index);
	virtual int getIndex();
	virtual void colorFocus(bool color);
	virtual void colorSelect(bool color);

	virtual int widthFromHeight(int height);

signals:
	void sMousePressEvent(QMouseEvent *event, int index);
	void sMouseReleaseEvent(QMouseEvent *event, int index);
	
protected:
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
		
private:
	int m_index = -1;
};


#endif