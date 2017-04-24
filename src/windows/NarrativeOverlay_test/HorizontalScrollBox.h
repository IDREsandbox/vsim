#ifndef HORIZONTALSCROLLBOX_TEST_H
#define HORIZONTALSCROLLBOX_TEST_H

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QDebug>
#include <QtWidgets/QMenu>
#include <QContextMenuEvent>

// goals:
// 1. add, insert, delete, select, clear (signals)
// 2. context menu for ^
// 3. copy paste, multiselect, multidelete, drag n drop, other fancy stuff

// configuration
// size, item width/height

class ScrollBoxItem : public QLabel {
public:
	ScrollBoxItem(QWidget* parent) : QLabel(parent) {
	};
protected:
	int i = 0;
	void mouseMoveEvent(QMouseEvent* event){
		qDebug() << "mouse move" << this->text() << event->modifiers();
		i++;
	}
	void mousePressEvent(QMouseEvent* event) {
		qDebug() << "mouse event" << this->text();
	}
	void mouseDoubleClickEvent(QMouseEvent* event) {
		qDebug() << "mouse double click" << this->text();
	}
};


class HorizontalScrollBox : public QScrollArea {
	Q_OBJECT
public:
	HorizontalScrollBox(QWidget* parent);

	// Items
	// TODO: replace text with QImage or something
	void addBlankItem();
	void addItem(QString text);
	void insertItem(int position, QString text);

	void deleteItem(int position);
	void deleteSelection();

	// selection
	void select(QWidget*);
	void deselect();

	//void slideContextMenu(QContextMenuEvent*);
	
	// internal slots
	// pos: global click position, slide: item index
	void onItemMenu(QPoint globalPos, ScrollBoxItem* widget);
	//void onSpaceMenu(int position);

signals:
	//void selected(int item);
	//void deleted();
	//void deleted(int);
	//void deleted(list);

protected:
	// qt overrides
	virtual void resizeEvent(QResizeEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

private:
	// use this to 
	void setWidgetWidth();
	int getIndexOf(QWidget*);
	void positionChildren();
	
	QWidget* m_scroll_area_widget;
	QList<QWidget*> m_items;

	QWidget* m_selection; // TODO: range selection

	//int boxHeight() const;
	//int boxWidth() const;
	int m_height = 100;
	int m_spacing = 10;
	float m_ratio = 1.0; // width to height ratio for items
	float m_space_ratio = .1; // TODO: width to height ratio for spaces

	QMenu* m_slide_menu; // context menu
};

#endif // HORIZONTALSCROLLBOX_TEST_H