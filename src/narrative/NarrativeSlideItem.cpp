#include "narrative/NarrativeSlideItem.h"
#include "Util.h"

NarrativeSlideItem::NarrativeSlideItem()
{
}

QRectF NarrativeSlideItem::getRect() const
{
	return m_rect;
}

void NarrativeSlideItem::setRect(QRectF rect)
{
	m_rect = rect;
	emit sTransformed(rect);
}

QColor NarrativeSlideItem::getBackground() const
{
	return m_color;
}

void NarrativeSlideItem::setBackground(const QColor &color)
{
	m_color = color;
	emit sBackgroundChanged(m_color);
}