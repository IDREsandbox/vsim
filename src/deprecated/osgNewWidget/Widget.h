/*
 * Widget.h
 *
 *  Created on: May 4, 2011
 *      Author: eduardo
 */

#ifndef OSGNW_WIDGET_H_
#define OSGNW_WIDGET_H_

#include <osg/Geode>
#include <osg/Geometry>
//#include "osgNewWidget/WidgetAnimator.h"

namespace osg
{
    class Geometry;
}

namespace osgNewWidget
{

class IWidgetEventHandler;
class Canvas;

enum WidgetFlags
{
    WIDGET_NONE = 0,
    WIDGET_WITH_GEOMETRY = 1 << 0,
    WIDGET_WITH_BORDER = 1 << 1
};

const unsigned int NODEMASK_2D = 0x00000001;
const unsigned int NODEMASK_3D = 0x00000002;

enum AnchorHEdge { ANCHOR_L, ANCHOR_LR, ANCHOR_R };
enum AnchorVEdge { ANCHOR_B, ANCHOR_BT, ANCHOR_T };

class Widget: public osg::Geode //, public IAnimatedWidget
{
public:
    Widget(int flags = osgNewWidget::WIDGET_NONE);
    Widget(const Widget&, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~Widget();

    META_Node(osgNewWidget, Widget);

    const osg::Vec2f& getPosition() const { return m_position; }
    virtual void setPosition(const osg::Vec2f& pos);
    const osg::Vec2f& getSize() const { return m_size; }
    virtual void setSize(const osg::Vec2f& size);
    float getAlphaMult() const { return m_alphaMult; }
    virtual void setAlphaMult(float a);
    osg::Vec4f getColor() const;
    void setColor(const osg::Vec4f& color);
    osg::Vec4f getBorderColor() const;
    void setBorderColor(const osg::Vec4f& color);
    void setImage(osg::Image* image);
    bool setImage(const std::string& filePath);
    void setHighlight(bool highlight);
	void setColors(osg::Vec4 color);
	void setAlpha(const float a);

    osgNewWidget::IWidgetEventHandler* getHandler() { return m_handler; }
    void setHandler(osgNewWidget::IWidgetEventHandler* handler);

    int getZOffset() const { return m_zoffset; }
    virtual void setZOffset(int zoffset);
    const osg::Vec2f& getParentOffset() const { return m_parentOffset; }
    virtual void setParentOffset(const osg::Vec2f& offset);

    const osgNewWidget::Widget* getParent() const { return m_parent; }
    osgNewWidget::Widget* getParent() { return m_parent; }
    void setParent(osgNewWidget::Widget* parent) { m_parent = parent; }

    //virtual float getAnimPropertyValue(AnimProperty animproperty);
    //virtual void setAnimPropertyValue(AnimProperty animproperty, float value);

    AnchorHEdge getAnchorHEdge() const { return m_anchorHEdge; }
    void setAnchorHEdge(AnchorHEdge val) { m_anchorHEdge = val; }
    AnchorVEdge getAnchorVEdge() const { return m_anchorVEdge; }
    void setAnchorVEdge(AnchorVEdge val) { m_anchorVEdge = val; }
    const osg::Vec2f& getAnchorValue() const { return m_anchorValue; }
    void setAnchorValue(const osg::Vec2f& anchorValue) { m_anchorValue = anchorValue; }
	
	// Used only by serializer
    const osg::Geometry* getBgGeometry() const;
    void setBgGeometry(osg::Geometry* geometry);
    const osg::Geometry* getBorderGeometry() const;
    void setBorderGeometry(osg::Geometry* geometry);

    virtual void show();
    virtual void hide();

protected:
    osg::Vec2f m_position;
    osg::Vec2f m_size;
    float m_alpha;
    float m_borderAlpha;
    float m_alphaMult;
    osg::ref_ptr<osg::Geometry> m_bgGeometry;
    osg::ref_ptr<osg::Geometry> m_borderGeometry;
    osgNewWidget::IWidgetEventHandler* m_handler;
    int m_zoffset;
    osg::Vec2f m_parentOffset;
    AnchorHEdge m_anchorHEdge;
    AnchorVEdge m_anchorVEdge;
    osg::Vec2f m_anchorValue;
    osgNewWidget::Widget* m_parent;
    bool m_highlight;

    void setTexture(osg::Texture* texture);
    void setInternalGeometryColor(const osg::Vec4f& color);
    void setInternalBorderColor(const osg::Vec4f& color);
};

}

#endif /* OSGNW_WIDGET_H_ */
