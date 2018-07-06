#ifndef SLIDESCROLLITEM_H
#define SLIDESCROLLITEM_H
#include "ScrollBoxItem.h"
#include "narrative/NarrativeSlide.h"
#include "ui_SlideScrollItem.h"

// Gui 'view' of narrative slide
class SlideScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	SlideScrollItem(QWidget *parent);
	void setSlide(NarrativeSlide *);
	NarrativeSlide *getSlide();

	// set gui
	void setImage(const QImage &img);
	void setTransition(float duration);
	void setDuration(bool stay, float duration); // 0 for stay, >0 for timed

	// thumbnail stuff
	//bool thumbnailDirty();
	//void setThumbnailDirty(bool dirty);

	// overrides 
	virtual void setIndex(int index) override;
	virtual int widthFromHeight(int height) const override;
	//virtual void colorFocus(bool color);
	//virtual void colorSelect(bool color);

	// for filtering double clicks
	bool eventFilter(QObject * obj, QEvent * ev);

signals:
	void sTransitionDoubleClick();
	void sDurationDoubleClick();
	//void sTransitionClick();

	void sThumbnailDirty();

private:
	Ui::SlideScrollItem ui;
	NarrativeSlide *m_slide; // pointer to the data

	bool m_thumbnail_dirty;
};

#endif // SLIDESCROLLITEM_HPP