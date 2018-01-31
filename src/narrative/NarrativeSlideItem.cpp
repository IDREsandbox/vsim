#include "narrative/NarrativeSlideItem.h"

NarrativeSlideItem::NarrativeSlideItem()
	: osg::Node()
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

const osg::Vec4 &NarrativeSlideItem::getXYWH() const
{
	m_rect_vec = osg::Vec4(m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height());
	return m_rect_vec;
}

void NarrativeSlideItem::setXYWH(const osg::Vec4 &vec)
{
	setRect(QRectF(vec[0], vec[1], vec[2], vec[3]));
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

const osg::Vec4 &NarrativeSlideItem::getBackgroundRGBA() const
{
	m_color_vec = osg::Vec4(m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height());
	return m_color_vec;
}

void NarrativeSlideItem::setBackgroundRGBA(const osg::Vec4 &vec)
{
	m_color = QColor(vec[0], vec[1], vec[2], vec[3]);
}