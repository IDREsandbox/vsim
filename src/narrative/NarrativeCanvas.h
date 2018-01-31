#ifndef NARRATIVECANVAS_H
#define NARRATIVECANVAS_H

#include <map>

#include "narrative/CanvasContainer.h"

class NarrativeSlideItem;
class NarrativeSlideLabel;
class NarrativeSlide;
class NarrativeCanvas : public CanvasContainer
{
	Q_OBJECT;
public:
	NarrativeCanvas(QWidget *parent = nullptr);

	void setSelection(std::set<NarrativeSlideItem*> items);
	std::set<NarrativeSlideItem*> getSelection() const;
	void setItemFocus(NarrativeSlideItem*);
	NarrativeSlideItem *getItemFocus() const;

	void setSlide(NarrativeSlide *slide);
	NarrativeSlide *getSlide() const;

	void enableEditing(bool enable);

signals:
	void sItemsTransformed(const std::map<NarrativeSlideItem*, QRectF> &new_rects);
	void sLabelEdited(NarrativeSlideLabel*);
	//void sDeleteLabel();

private:
	NarrativeSlide * m_slide;

	bool m_editing;
};

#endif
