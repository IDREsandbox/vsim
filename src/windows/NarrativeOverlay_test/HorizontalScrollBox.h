#ifndef HORIZONTALSCROLLBOX_TEST_H
#define HORIZONTALSCROLLBOX_TEST_H

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
// goals:
// 1. add, insert, delete, select, clear
// 2. context menu for ^
// 3. multiselect, multidelete, drag n drop, other fancy stuff

// configuration
// size, item width/height

class FixedRatioLayout : public QHBoxLayout {
public:
	//bool hasHeightForWidth() { return true; }
	//int heightForWidth(int w) { return 3*w/4; }
};

class HorizontalScrollBox : public QScrollArea {
	Q_OBJECT
public:
	HorizontalScrollBox(QWidget* parent);

	void add();
	void insert();


//public signals:
//	void selected();
//	void deleted();

private:
	QWidget* m_scroll_area_widget;
	QHBoxLayout* m_scroll_area_layout;

};

#endif // HORIZONTALSCROLLBOX_TEST_H