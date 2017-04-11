#ifndef HORIZONTALSCROLLBOX_TEST_H
#define HORIZONTALSCROLLBOX_TEST_H

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

//#include "NarrativeLayout.h"
// goals:
// 1. add, insert, delete, select, clear (signals)
// 2. context menu for ^
// 3. multiselect, multidelete, drag n drop, other fancy stuff

// configuration
// size, item width/height

class HorizontalScrollBox : public QScrollArea {
	Q_OBJECT
public:
	HorizontalScrollBox(QWidget* parent);

	void add();
	void insert();

protected:
	void resizeEvent(QResizeEvent* event);

//public signals:
//	void selected();
//	void deleted();

private:
	QWidget* m_scroll_area_widget;
	QHBoxLayout* m_scroll_area_layout;
	//CardLayout* m_card_layout;
};

#endif // HORIZONTALSCROLLBOX_TEST_H