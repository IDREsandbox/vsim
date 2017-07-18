/*
 * Canvas.cpp
 *
 *  Created on: May 4, 2011
 *      Author: eduardo
 */

#include "osgNewWidget/Canvas.h"
#include <osg/Scissor>

namespace osgNewWidget
{

#define FOR_EACH_CHILD(x) \
    { \
        int numChildren = m_group->getNumChildren(); \
        for (int i = 0; i < numChildren; i++) \
        { \
            osg::Node* n = m_group->getChild(i); \
            osgNewWidget::Widget* (x) = dynamic_cast<osgNewWidget::Widget*>(n); \
            if (x) \
            {
#define END_FOR_EACH \
            } \
        } \
    }

Canvas::Canvas(int flags): Widget(flags), m_contentOffset(0.0f, 0.0f),
        m_parentScissorPos(0.0f, 0.0f), m_parentScissorSize(FLT_MAX, FLT_MAX)
{
    m_group = new osg::Group();
    //m_group->setNodeMask(osgNewWidget::NODEMASK_2D);
    m_group->getOrCreateStateSet()->setAttributeAndModes(new osg::Scissor(0, 0, 0, 0),
        osg::StateAttribute::ON);
}

Canvas::Canvas(const Canvas& canvas, const osg::CopyOp& copyop):
    Widget(canvas, copyop)
{
    m_group = new osg::Group(*canvas.m_group.get(), copyop);
}

Canvas::~Canvas()
{
}

void Canvas::addWidget(osgNewWidget::Widget* widget)
{
    osg::Vec2f canvaspos = getPosition() + getParentOffset();
    widget->setParentOffset(canvaspos);
    widget->setZOffset(getZOffset() + 1);
    m_group->addChild(widget);
    osgNewWidget::Canvas* canvas = dynamic_cast<osgNewWidget::Canvas*>(widget);
    if (canvas)
    {
        canvas->setParentScissors(canvaspos, getSize());
        m_group->addChild(canvas->getGroup());
    }
    widget->setParent(this);
}

void Canvas::removeWidget(osgNewWidget::Widget* widget)
{
    widget->setParentOffset(osg::Vec2f(0.0f, 0.0f));
    widget->setParent(NULL);
    osgNewWidget::Canvas* canvas = dynamic_cast<osgNewWidget::Canvas*>(widget);
    if (canvas)
    {
        canvas->setParentScissors(osg::Vec2f(0.0f, 0.0f), osg::Vec2f(FLT_MAX, FLT_MAX));
        m_group->removeChild(canvas->getGroup());
    }
    m_group->removeChild(widget);
}

void Canvas::setParentScissors(const osg::Vec2f& pos, const osg::Vec2f& size)
{
    m_parentScissorPos = pos;
    m_parentScissorSize = size;
    updateScissors();
}

void Canvas::updateScissors()
{
    osg::Vec2f size = getSize();
    osg::Vec2f pos = getPosition() + getParentOffset();
    if (pos.x() < m_parentScissorPos.x())
        pos.x() = m_parentScissorPos.x();
    if (pos.y() < m_parentScissorPos.y())
        pos.y() = m_parentScissorPos.y();
    if (pos.x() + size.x() > m_parentScissorPos.x() + m_parentScissorSize.x())
        pos.x() = m_parentScissorPos.x() + m_parentScissorSize.x() - size.x();
    if (pos.y() + size.y() > m_parentScissorPos.y() + m_parentScissorSize.y())
        pos.y() = m_parentScissorPos.y() + m_parentScissorSize.y() - size.y();
    dynamic_cast<osg::Scissor*>(m_group->getStateSet()->getAttribute(osg::StateAttribute::SCISSOR))
        ->setScissor(pos.x(), pos.y(), size.x(), size.y());
    //std::cout << getName() << " Scissors " << pos.x() << " " << pos.y() << " " << size.x() << " " << size.y() << std::endl;
    FOR_EACH_CHILD(w)
    {
        osgNewWidget::Canvas* c = dynamic_cast<osgNewWidget::Canvas*>(w);
        if (c)
            c->setParentScissors(pos, size);
    }
    END_FOR_EACH
}

void Canvas::setPosition(const osg::Vec2f& pos)
{
    Widget::setPosition(pos);
    setChildrenOffset();
    updateScissors();
}

void Canvas::setSize(const osg::Vec2f& size)
{
    Widget::setSize(size);
    FOR_EACH_CHILD(w)
    {
        if (w->getAnchorHEdge() == osgNewWidget::ANCHOR_LR)
        {
            float sx = size.x() - w->getPosition().x() - w->getAnchorValue().x();
            w->setSize(osg::Vec2f(sx, w->getSize().y()));
        }
        else if (w->getAnchorHEdge() == osgNewWidget::ANCHOR_R)
        {
            float px = size.x() - w->getSize().x() - w->getAnchorValue().x();
            w->setPosition(osg::Vec2f(px, w->getPosition().y()));
        }
        if (w->getAnchorVEdge() == osgNewWidget::ANCHOR_BT)
        {
            float sy = size.y() - w->getPosition().y() - w->getAnchorValue().y();
            w->setSize(osg::Vec2f(w->getSize().x(), sy));
        }
        else if (w->getAnchorVEdge() == osgNewWidget::ANCHOR_T)
        {
            float py = size.y() - w->getSize().y() - w->getAnchorValue().y();
            w->setPosition(osg::Vec2f(w->getPosition().x(), py));
        }
    }
    END_FOR_EACH
    updateScissors();
}

void Canvas::setAlphaMult(float a)
{
    Widget::setAlphaMult(a);
    FOR_EACH_CHILD(w)
    {
        w->setAlphaMult(a);
    }
    END_FOR_EACH
}

void Canvas::setZOffset(int zoffset)
{
    int delta = zoffset - getZOffset();
    Widget::setZOffset(zoffset);
    FOR_EACH_CHILD(w)
    {
        w->setZOffset(w->getZOffset() + delta);
    }
    END_FOR_EACH
}

void Canvas::setChildrenOffset()
{
    osg::Vec2f canvaspos = getPosition() + getParentOffset() + getContentOffset();
    FOR_EACH_CHILD(w)
    {
        w->setParentOffset(canvaspos);
    }
    END_FOR_EACH
}

void Canvas::setHAnchor(osgNewWidget::Widget* widget, AnchorHEdge anchorH)
{
    FOR_EACH_CHILD(w)
    {
        if (widget == w)
        {
            w->setAnchorHEdge(anchorH);
            osg::Vec2f v = w->getAnchorValue();
            v.x() = getSize().x() - w->getPosition().x() - w->getSize().x();
            w->setAnchorValue(v);
            break;
        }
    }
    END_FOR_EACH
}

void Canvas::setVAnchor(osgNewWidget::Widget* widget, AnchorVEdge anchorV)
{
    FOR_EACH_CHILD(w)
    {
        if (widget == w)
        {
            w->setAnchorVEdge(anchorV);
            osg::Vec2f v = w->getAnchorValue();
            v.y() = getSize().y() - w->getPosition().y() - w->getSize().y();
            w->setAnchorValue(v);
            break;
        }
    }
    END_FOR_EACH
}

void Canvas::setContentOffset(const osg::Vec2f& offset)
{
    m_contentOffset = offset;
    setChildrenOffset();
}
//
//float Canvas::getAnimPropertyValue(AnimProperty animproperty)
//{
//    switch(animproperty)
//    {
//    case ANIM_PROPERTY_CONTENT_OFFSET_X:
//        return getContentOffset().x();
//    case ANIM_PROPERTY_CONTENT_OFFSET_Y:
//        return getContentOffset().y();
//    case ANIM_PROPERTY_ALPHA_MULT:
//        return getAlphaMult();
//    }
//    return Widget::getAnimPropertyValue(animproperty);
//}
//
//void Canvas::setAnimPropertyValue(AnimProperty animproperty, float value)
//{
//    osg::Vec2f size;
//    osg::Vec2f pos;
//    switch(animproperty)
//    {
//    case ANIM_PROPERTY_CONTENT_OFFSET_X:
//        pos = getContentOffset();
//        pos.x() = floor(value);
//        setContentOffset(pos);
//        return;
//    case ANIM_PROPERTY_CONTENT_OFFSET_Y:
//        pos = getContentOffset();
//        pos.y() = floor(value);
//        setContentOffset(pos);
//        return;
//    case ANIM_PROPERTY_ALPHA_MULT:
//        setAlphaMult(value);
//        return;
//    }
//    return Widget::setAnimPropertyValue(animproperty, value);
//}

void Canvas::show()
{
	Widget::show();
	m_group->setNodeMask(~0x0u);
}

void Canvas::hide()
{
	Widget::hide();
	m_group->setNodeMask(0u);
}

}
