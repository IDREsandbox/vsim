#ifndef SCROLLBOXITEM_H
#define SCROLLBOXITEM_H

#include <QFrame>
#include <QWidget>
#include <QMouseEvent>
#include <QDebug>
#include <QColor>

class ScrollBoxItem : public QFrame {
	Q_OBJECT
public:
	ScrollBoxItem(QWidget *parent);
	
	// interface for controller is in derived classes
	
	// interface for gui
	// index is used for pretty numbering and selection signaling
	virtual void setIndex(int index);
	virtual int getIndex();

	void select(bool s);

	void setDeselectStyle(QString style);
	void setSelectStyle(QString style);

	// Palette is a pain
	//void setColor(QColor c);
	//void setSelectColor(QColor c);

	virtual int widthFromHeight(int height);

signals:
	void sMousePressEvent(QMouseEvent *event, int index);
	void sMouseReleaseEvent(QMouseEvent *event, int index);

protected:
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

private:
	int m_index = -1;

	bool m_selected;

	QString m_style;
	QString m_select_style;
	//QColor m_color;
	//QColor m_select_color;

};


#endif