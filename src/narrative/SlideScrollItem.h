#ifndef SLIDESCROLLITEM_H
#define SLIDESCROLLITEM_H
#include "ScrollBoxItem.h"
#include "ui_SlideScrollItem.h"

class SlideScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	SlideScrollItem();

	virtual void setIndex(int index);
	virtual int widthFromHeight(int height);
	virtual void colorFocus(bool color);
	virtual void colorSelect(bool color);

	virtual void resizeEvent(QResizeEvent* event);
	

private:
	Ui::SlideScrollItem ui;
};

#endif // SLIDESCROLLITEM_HPP