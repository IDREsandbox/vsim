#ifndef SLIDESCROLLITEM_H
#define SLIDESCROLLITEM_H

#include <QLabel>
#include <QVBoxLayout>
#include <string>
#include "ScrollBoxItem.h"
#include "Narrative.h"

#include "SlideTransitionScrollWidget.h"
#include "SlideScrollWidget.h"

class SlideScrollItem : public ScrollBoxItem {
	Q_OBJECT
public:
	SlideScrollItem();

	virtual void setIndex(int index);
	virtual int widthFromHeight(int height);
	
	SlideScrollWidget *getSlideWidget();
	SlideTransitionScrollWidget *getTransitionWidget();

protected:
	virtual void mousePressEvent(QMouseEvent *event);

private:
	QHBoxLayout *m_layout;
	
	SlideScrollWidget *m_slide_widget;
	SlideTransitionScrollWidget *m_trans_widget;
};

#endif