#include "NarrativeCanvas.h"

#include <QKeyEvent>
#include <QApplication>

#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeSlideLabel.h"

NarrativeCanvas::NarrativeCanvas(QWidget * parent)
	: CanvasContainer(parent),
	m_slide(nullptr)
{
	// forward transform signals
	connect(m_scene, &CanvasScene::rectsTransformed, this,
		[this](const std::map<RectItem*, QRectF> &rect_rects) {
		std::map<NarrativeSlideItem*, QRectF> item_rects;
		for (auto ritem_rect : rect_rects) {
			RectItem *ritem = ritem_rect.first;
			QRectF rect = ritem_rect.second;

			NarrativeSlideItem *item = canvasToSlide(ritem_rect.first);
			if (!item) continue;
			item_rects.insert(std::make_pair(item, rect));
		}
		emit sItemsTransformed(item_rects);
	});
}

void NarrativeCanvas::setSelection(const std::set<NarrativeSlideItem*> &items)
{
	std::set<RectItem*> rects;
	for (auto item : items) {
		auto rect = slideToCanvas(item);
		if (!rect) continue;
		rects.insert(rect);
	}
	m_scene->setSelectedRects(rects);
}

std::set<NarrativeSlideItem*> NarrativeCanvas::getSelection() const
{
	const std::set<RectItem*> &rects = m_scene->getSelectedRects();
	std::set<NarrativeSlideItem*> items;
	for (auto rect : rects) {
		// look for item in map
		auto item = canvasToSlide(rect);
		if (!item) continue;
		items.insert(item);
	}
	return items;
}

void NarrativeCanvas::setItemFocus(NarrativeSlideItem* item)
{
	m_scene->setFocusItem(slideToCanvas(item));
}

NarrativeSlideItem* NarrativeCanvas::getItemFocus() const
{
	RectItem *rect = dynamic_cast<RectItem*>(m_scene->focusItem());
	auto slide = canvasToSlide(rect);
	return canvasToSlide(rect);
}

RectItem * NarrativeCanvas::slideToCanvas(NarrativeSlideItem * item) const
{
	auto it = m_item_to_rect.find(item);
	if (it == m_item_to_rect.end()) return nullptr;
	return it->second;
}

NarrativeSlideItem * NarrativeCanvas::canvasToSlide(RectItem * rect) const
{
	auto it = m_rect_to_item.find(rect);
	if (it == m_rect_to_item.end()) return nullptr;
	return it->second;
}

void NarrativeCanvas::setSlide(NarrativeSlide *slide)
{
	if (m_slide) disconnect(m_slide, 0, this, 0);
	// disconnect all labels
	for (auto &item_rect : m_item_to_rect) {
		NarrativeSlideItem *item = item_rect.first;
		disconnect(item, 0, this, 0);
	}

	m_slide = slide;
	m_item_to_rect.clear();
	m_rect_to_item.clear();
	CanvasContainer::clear();
	if (!slide) return;

	for (uint i = 0; i < slide->getNumChildren(); i++) {
		NarrativeSlideItem *item = dynamic_cast<NarrativeSlideItem*>(m_slide->child(i));
		if (item) addItem(item);
	}
	show();

	connect(slide, &NarrativeSlide::sAddedP, this,
		[this](const std::set<osg::Node*> &nodes) {
		for (auto node : nodes) {
			NarrativeSlideItem *item = dynamic_cast<NarrativeSlideItem*>(node);
			if (item) addItem(item);
		}
	});
	connect(slide, &NarrativeSlide::sAboutToRemoveP, this,
		[this](const std::set<osg::Node*> &nodes) {
		for (auto node : nodes) {
			NarrativeSlideItem *item = dynamic_cast<NarrativeSlideItem*>(node);
			if (item) removeItem(item);
		}
	});
}

NarrativeSlide *NarrativeCanvas::getSlide() const
{
	return m_slide;
}

void NarrativeCanvas::addItem(NarrativeSlideItem *item)
{
	RectItem *rect;
	TextRect *text;
	NarrativeSlideLabel *label = dynamic_cast<NarrativeSlideLabel*>(item);
	if (label) {
		// make a text rect
		text = new TextRect();
		rect = text;
		text->m_text->setDocument(label->getDocument());
		text->setBaseHeight(m_base_height);

		connect(text->m_text->document(), &QTextDocument::undoCommandAdded, this,
			[this, text]() {
			NarrativeSlideLabel *item = dynamic_cast<NarrativeSlideLabel*>(canvasToSlide(text));
			if (!item) return;
			emit sLabelUndoCommandAdded(item);
		});
	}
	else {
		rect = new RectItem();
	}

	connect(item, &NarrativeSlideItem::sTransformed, this,
		[this, rect](QRectF r) {
		rect->setRect(r);
	});

	connect(item, &NarrativeSlideItem::sBackgroundChanged, this,
		[this, rect](QColor color) {
		rect->setBrush(color);
	});

	rect->setRect(label->getRect());
	rect->setBrush(label->getBackground());
	rect->setEditable(isEditable());
	m_scene->addItem(rect);

	m_item_to_rect[item] = rect;
	m_rect_to_item[rect] = item;
}

void NarrativeCanvas::removeItem(NarrativeSlideItem *item)
{
	disconnect(item, 0, this, 0);

	// item to rect iterator
	auto it = m_item_to_rect.find(item);
	if (it == m_item_to_rect.end()) {
		qWarning() << "Error during narrative slide item delete - no canvas item for index";
		return;
	}	
	RectItem *rect = it->second;

	delete rect;
	m_item_to_rect.erase(item);
	m_rect_to_item.erase(rect);
}

