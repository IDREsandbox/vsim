#ifndef SLIDESCROLLITEM_H
#define SLIDESCROLLITEM_H
#include "ScrollBoxItem.h"
#include "ui_SlideScrollItem.h"

class SlideScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	SlideScrollItem();

	// 
	void setTransition(float);
	void setDuration(float);

	// overrides 
	virtual void setIndex(int index);
	virtual int widthFromHeight(int height);
	virtual void colorFocus(bool color);
	virtual void colorSelect(bool color);

	// for filtering double clicks
	bool eventFilter(QObject * obj, QEvent * ev);

signals:
	void sTransitionDoubleClick();

private:
	Ui::SlideScrollItem ui;
};

#endif // SLIDESCROLLITEM_HPP