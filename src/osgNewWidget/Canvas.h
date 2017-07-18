/*
 * Canvas.h
 *
 *  Created on: May 4, 2011
 *      Author: eduardo
 */

#ifndef OSGNW_CANVAS_H_
#define OSGNW_CANVAS_H_

#include "osgNewWidget/Widget.h"

namespace osgNewWidget
{

class Canvas: public Widget
{
public:
    Canvas(int flags = osgNewWidget::WIDGET_NONE);
    Canvas(const Canvas&, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~Canvas();

    META_Node(osgNewWidget, Canvas);

    virtual void addWidget(osgNewWidget::Widget* widget);
    virtual void removeWidget(osgNewWidget::Widget* widget);
    virtual void setPosition(const osg::Vec2f& pos);
    virtual void setSize(const osg::Vec2f& size);
    virtual void setAlphaMult(float a);
    osg::Group* getGroup() { return m_group.get(); }
    const osg::Group* getGroup() const { return m_group.get(); }
    void setGroup(osg::Group* group) { m_group = group; }
    virtual void setZOffset(int zoffset);
    void setHAnchor(osgNewWidget::Widget* widget, AnchorHEdge anchorH);
    void setVAnchor(osgNewWidget::Widget* widget, AnchorVEdge anchorV);
    osg::Vec2f getContentOffset() { return m_contentOffset; }
    virtual void setContentOffset(const osg::Vec2f& offset);
    void setParentScissors(const osg::Vec2f& pos, const osg::Vec2f& size);

    //virtual float getAnimPropertyValue(AnimProperty animproperty);
    //virtual void setAnimPropertyValue(AnimProperty animproperty, float value);

    virtual void show();
    virtual void hide();

protected:
    osg::ref_ptr<osg::Group> m_group;
    osg::Vec2f m_contentOffset;
    osg::Vec2f m_parentScissorPos;
    osg::Vec2f m_parentScissorSize;

    void setChildrenOffset();
    void updateScissors();
};

}

#endif /* OSGNW_CANVAS_H_ */
