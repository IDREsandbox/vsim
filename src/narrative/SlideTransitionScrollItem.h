#ifndef SLIDETRANSITIONSCROLLITEM_H
#define SLIDETRANSITIONSCROLLITEM_H

#include <QLabel>
#include <QVBoxLayout>
#include "ScrollBoxItem.h"

class SlideTransitionScrollItem : public ScrollBoxItem {
	Q_OBJECT
public:
	SlideTransitionScrollItem();
	virtual int widthFromHeight(int height);

private:
	QLabel *m_duration_label;
	QVBoxLayout *m_layout;
};

#endif