#include "NarrativeCanvas.h"

#include <QKeyEvent>
#include <QApplication>

#include "narrative/NarrativeSlide.h"

NarrativeCanvas::NarrativeCanvas(QWidget * parent)
	: CanvasContainer(parent),
	m_slide(nullptr)
{
}

void NarrativeCanvas::setSelection(std::set<NarrativeSlideItem*> indices)
{

}

std::set<NarrativeSlideItem*> NarrativeCanvas::getSelection() const
{
	return {};
}

void NarrativeCanvas::setItemFocus(NarrativeSlideItem* item)
{

}

NarrativeSlideItem* NarrativeCanvas::getItemFocus() const
{
	return 0;
}

void NarrativeCanvas::setSlide(NarrativeSlide *slide)
{
	if (m_slide) disconnect(m_slide, 0, this, 0);
	m_slide = slide;
	clear();
	if (!slide) return;

	for (uint i = 0; i < slide->getNumChildren(); i++) {
		//insertNewLabel(i);
	}
	show();

	connect(slide, &NarrativeSlide::sNew, this,
		[this](int index) {
		qDebug() << "insert new slide" << index;
	});
	connect(slide, &NarrativeSlide::sDelete, this,
		[this](int index) {
		qDebug() << "delete slide" << index;
	});
}

NarrativeSlide *NarrativeCanvas::getSlide() const
{
	return m_slide;
}

void NarrativeCanvas::enableEditing(bool enable)
{
	m_editing = enable;
}
