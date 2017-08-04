#ifndef SLIDESCROLLITEM_H
#define SLIDESCROLLITEM_H
#include "ScrollBoxItem.h"
#include "narrative/NarrativeSlide.h"
#include "ui_SlideScrollItem.h"

// Gui 'view' of narrative slide
class SlideScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	SlideScrollItem();
	SlideScrollItem(NarrativeSlide *);
	void setSlide(NarrativeSlide *);

	void setImage(const QImage &img);

	// Scroll Box Iterface
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
	NarrativeSlide *m_slide; // pointer to the data

	bool m_thumbnail_dirty;
};

#endif // SLIDESCROLLITEM_HPP