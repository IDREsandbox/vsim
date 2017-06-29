#ifndef SLIDESCROLLITEM_H
#define SLIDESCROLLITEM_H
#include "ScrollBoxItem.h"
#include "ui_SlideScrollItem.h"

class SlideScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	SlideScrollItem();

	float getTransition();
	float getDuration();
	void setTransition(float);
	void setDuration(float); // 0 for stay, >0 for timed

	// overrides 
	virtual void setIndex(int index);
	virtual int widthFromHeight(int height);
	virtual void colorFocus(bool color);
	virtual void colorSelect(bool color);

	// for filtering double clicks
	bool eventFilter(QObject * obj, QEvent * ev);

signals:
	void sTransitionDoubleClick();
	void sDurationDoubleClick();

private:
	Ui::SlideScrollItem ui;

	float m_duration;
	float m_transition_duration;
};

#endif // SLIDESCROLLITEM_HPP