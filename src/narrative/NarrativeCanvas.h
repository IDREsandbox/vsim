#ifndef NARRATIVECANVAS_H
#define NARRATIVECANVAS_H

#include <map>
#include <set>
#include "narrative/CanvasContainer.h"

class NarrativeSlideItem;
class NarrativeSlideLabel;
class NarrativeSlide;
class NarrativeCanvas : public CanvasContainer
{
	Q_OBJECT;
public:
	NarrativeCanvas(QWidget *parent = nullptr);

	void setSlide(NarrativeSlide *slide);
	NarrativeSlide *getSlide() const;

	// maps canvas rects <=> slide items
	void setSelection(const std::set<NarrativeSlideItem*> &items);
	std::set<NarrativeSlideItem*> getSelection() const;

	void setItemFocus(NarrativeSlideItem* item);
	NarrativeSlideItem* getItemFocus() const;

	NarrativeSlideItem *canvasToSlide(RectItem *rect) const;
	RectItem *slideToCanvas(NarrativeSlideItem *item) const;

signals:
	void sPoked();
	void sItemsTransformed(const std::map<NarrativeSlideItem*, QRectF> &new_rects);
	void sLabelUndoCommandAdded(NarrativeSlideLabel*);

private:
	void addItem(NarrativeSlideItem *item);
	void removeItem(NarrativeSlideItem *item);

private:
	NarrativeSlide *m_slide;

	bool m_editing;

	std::map<NarrativeSlideItem*, RectItem*> m_item_to_rect;
	std::map<RectItem*, NarrativeSlideItem*> m_rect_to_item;
};


#endif
