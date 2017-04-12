#ifndef HORIZONTALSCROLLBOX_TEST_H
#define HORIZONTALSCROLLBOX_TEST_H

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QDebug>

// goals:
// 1. add, insert, delete, select, clear (signals)
// 2. context menu for ^
// 3. multiselect, multidelete, drag n drop, other fancy stuff

// configuration
// size, item width/height

class ScrollBoxItem : public QLabel {
public:
	ScrollBoxItem(QWidget* parent) : QLabel(parent) {};
protected:
	int i = 0;
	void mouseMoveEvent(QMouseEvent* event){
		qDebug() << "mouse move" << i;
		i++;
	}
};

class HorizontalScrollBox : public QScrollArea {
	Q_OBJECT
public:
	HorizontalScrollBox(QWidget* parent);

	// TODO: replace text with QImage or something
	void addItem(QString text);
	void insertItem(int position, QString text);


signals:
	void selected(int);
	//	void selected();
	//	void deleted();


protected:
	void positionChildren();
	void resizeEvent(QResizeEvent* event);


private:
	QWidget* m_scroll_area_widget;
	QList<QWidget*> m_items;

	//int boxHeight() const;
	//int boxWidth() const;
	int m_height = 100;
	int m_spacing = 10;
	int m_ratio = 1.0;
};

#endif // HORIZONTALSCROLLBOX_TEST_H