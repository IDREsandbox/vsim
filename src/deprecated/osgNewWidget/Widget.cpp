/*
 * Widget.cpp
 *
 *  Created on: May 4, 2011
 *      Author: eduardo
 */

#include "deprecated/osgNewWidget/Widget.h"
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/LineWidth>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

namespace osgNewWidget
{

Widget::Widget(int flags)
	: osg::Geode(), 
	m_position(0.0f, 0.0f),
	m_size(0.0f,
	0.0f),
	m_alpha(1.0f),
	m_borderAlpha(1.0f),
	m_alphaMult(1.0f),
	m_bgGeometry(NULL),
	m_borderGeometry(NULL),
	m_handler(NULL),
	m_zoffset(0),
	m_parentOffset(0.0f,
	0.0f),
	m_anchorHEdge(ANCHOR_L),
	m_anchorVEdge(ANCHOR_B),
	m_anchorValue(0.0f,
	0.0f),
	m_parent(NULL),
	m_highlight(false)
{
    if (flags & osgNewWidget::WIDGET_WITH_GEOMETRY)
    {
        m_bgGeometry = new osg::Geometry();
        m_bgGeometry->setName("wgeom");
        osg::Vec3 coords[] =
        {
            osg::Vec3(0.0, 0.0, 0.0),
            osg::Vec3(0.0, 0.0, 0.0),
            osg::Vec3(0.0, 0.0, 0.0),
            osg::Vec3(0.0, 0.0, 0.0)
        };

        m_bgGeometry->setUseDisplayList(false);
        m_bgGeometry->setDataVariance(osg::Object::DYNAMIC);

        int numCoords = sizeof(coords)/sizeof(osg::Vec3);
        osg::Vec3Array* vertices = new osg::Vec3Array(numCoords, coords);

        m_bgGeometry->setVertexArray(vertices);

        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));

        m_bgGeometry->setColorArray(colors);
        m_bgGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

        osg::Vec3Array* normals = new osg::Vec3Array;
        normals->push_back(osg::Vec3(0.0, 0.0, 1.0));

        m_bgGeometry->setNormalArray(normals);
        m_bgGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

        m_bgGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, numCoords));

        osg::Vec2 texs[] =
        {
            osg::Vec2(0.0, 0.0),
            osg::Vec2(1.0, 0.0),
            osg::Vec2(1.0, 1.0),
            osg::Vec2(0.0, 1.0)
        };
        int numTexs = sizeof(texs)/sizeof(osg::Vec2);

        osg::Vec2Array* texsArray = new osg::Vec2Array(numTexs, texs);
        m_bgGeometry->setTexCoordArray(0, texsArray);

        addDrawable(m_bgGeometry);
    }
    if (flags & osgNewWidget::WIDGET_WITH_BORDER)
    {
        m_borderGeometry = new osg::Geometry();
        m_borderGeometry->setName("wbgeom");
        osg::Vec3Array& a = *static_cast<osg::Vec3Array*>(m_bgGeometry->getVertexArray());
        osg::Vec3 offset(-0.5, -0.5, 0.0);
        osg::Vec3 coords[] =
        {
            osg::Vec3(a[0] + offset),
            osg::Vec3(a[1] + offset),
            osg::Vec3(a[2] + offset),
            osg::Vec3(a[3] + offset)
        };

        m_borderGeometry->setUseDisplayList(false);
        m_borderGeometry->setDataVariance(osg::Object::DYNAMIC);

        int numCoords = sizeof(coords)/sizeof(osg::Vec3);
        for (int i = 0; i < numCoords; i++)
            coords[i].z() += 0.01;
        osg::Vec3Array* vertices = new osg::Vec3Array(numCoords, coords);

        m_borderGeometry->setVertexArray(vertices);

        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));

        m_borderGeometry->setColorArray(colors);
        m_borderGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

        osg::Vec3Array* normals = new osg::Vec3Array;
        normals->push_back(osg::Vec3(0.0, 0.0, 1.0));

        m_borderGeometry->setNormalArray(normals);
        m_borderGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

        m_borderGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, numCoords));
        m_borderGeometry->getOrCreateStateSet()->setAttribute(new osg::LineWidth(1.0f));

        addDrawable(m_borderGeometry);
    }

    //setNodeMask(osgNewWidget::NODEMASK_2D);
}

Widget::Widget(const Widget& widget, const osg::CopyOp& copyop):
    osg::Geode(widget, copyop), m_position(widget.m_position), m_size(widget.m_size),
    m_alphaMult(1.0f), m_bgGeometry(widget.m_bgGeometry), m_borderGeometry(widget.m_borderGeometry),
    m_handler(widget.m_handler), m_zoffset(widget.m_zoffset), m_parentOffset(widget.m_parentOffset),
    m_anchorHEdge(widget.m_anchorHEdge), m_anchorVEdge(widget.m_anchorVEdge),
    m_anchorValue(widget.m_anchorValue), m_parent(widget.m_parent)
{
}

Widget::~Widget()
{
}

void Widget::setPosition(const osg::Vec2f& pos)
{
    m_position = pos;
    float lx = pos.x() + m_parentOffset.x();
    float ly = pos.y() + m_parentOffset.y();
    osg::Vec2f s = getSize();
    if (m_bgGeometry.get())
    {
        osg::Vec3Array& a = *static_cast<osg::Vec3Array*>(m_bgGeometry->getVertexArray());
        a[0].x() = lx;
        a[0].y() = ly;
        a[1].x() = lx + s.x();
        a[1].y() = ly;
        a[2].x() = lx + s.x();
        a[2].y() = ly + s.y();
        a[3].x() = lx;
        a[3].y() = ly + s.y();
        m_bgGeometry->dirtyBound();
    }
    if (m_borderGeometry.get())
    {
        lx = floor(lx) - 0.5f;
        ly = floor(ly) - 0.5f;
        osg::Vec3Array& b = *static_cast<osg::Vec3Array*>(m_borderGeometry->getVertexArray());
        b[0].x() = lx;
        b[0].y() = ly;
        b[1].x() = lx + s.x();
        b[1].y() = ly;
        b[2].x() = lx + s.x();
        b[2].y() = ly + s.y();
        b[3].x() = lx;
        b[3].y() = ly + s.y();
        m_borderGeometry->dirtyBound();
    }
}

void Widget::setSize(const osg::Vec2f& size)
{
    m_size = size;
    float lx = size.x() + m_parentOffset.x();
    float ly = size.y() + m_parentOffset.y();
    osg::Vec2f p = getPosition();
    if (m_bgGeometry.get())
    {
        osg::Vec3Array& a = *static_cast<osg::Vec3Array*>(m_bgGeometry->getVertexArray());
        a[1].x() = p.x() + lx;
        a[2].x() = p.x() + lx;
        a[2].y() = p.y() + ly;
        a[3].y() = p.y() + ly;
        m_bgGeometry->dirtyBound();
    }
    if (m_borderGeometry.get())
    {
        lx = floor(lx) - 0.5f;
        ly = floor(ly) - 0.5f;
        osg::Vec3Array& b = *static_cast<osg::Vec3Array*>(m_borderGeometry->getVertexArray());
        b[1].x() = p.x() + lx;
        b[2].x() = p.x() + lx;
        b[2].y() = p.y() + ly;
        b[3].y() = p.y() + ly;
        m_borderGeometry->dirtyBound();
    }
}
	
// Used only by serializer.
const osg::Geometry* Widget::getBgGeometry() const
{
	//OSG_ALWAYS << "getBgGeometry" << std::endl;
	// Update the object's alpha so that it's saved correctly.
	osgNewWidget::Widget* mutableThis = const_cast<osgNewWidget::Widget*>(this);
	mutableThis->setAlphaMult(1.0f);
	return m_bgGeometry;
}

// Used only by serializer.
void Widget::setBgGeometry(osg::Geometry* geometry)
{
	m_bgGeometry = geometry;
	osg::Vec4Array& colors = *static_cast<osg::Vec4Array*>(m_bgGeometry->getColorArray());
	osg::Vec4f color = colors.back();
	m_alpha = color.a();
}

// Used only by serializer.
const osg::Geometry* Widget::getBorderGeometry() const
{
	// OSG_ALWAYS << "getBorderGeometry" << std::endl;
	// Update the object's alpha so that it's saved correctly.
	osgNewWidget::Widget* mutableThis = const_cast<osgNewWidget::Widget*>(this);
	mutableThis->setAlphaMult(1.0f);
	return m_borderGeometry;
}

// Used only by serializer.
void Widget::setBorderGeometry(osg::Geometry* geometry)
{
	m_borderGeometry = geometry;
	osg::Vec4Array& colors = *static_cast<osg::Vec4Array*>(m_borderGeometry->getColorArray());
	osg::Vec4f color = colors.back();
	m_borderAlpha = color.a();
}

osg::Vec4f Widget::getColor() const
{
    osg::Vec4Array& colors = *static_cast<osg::Vec4Array*>(m_bgGeometry->getColorArray());
	osg::Vec4f color = colors.back();
	// Publicly visible color includes the original alpha, not the (possibly animated) current one
	color.a() = m_alpha;
    return color;
}

void Widget::setInternalGeometryColor(const osg::Vec4f& color)
{
    osg::Vec4Array& colors = *static_cast<osg::Vec4Array*>(m_bgGeometry->getColorArray());
    colors.back() = color;
}

void Widget::setColor(const osg::Vec4f& color)
{
    setInternalGeometryColor(color);
    m_alpha = color.a();
}

void Widget::setAlpha(const float a)
{
	m_borderAlpha = a;
}

osg::Vec4f Widget::getBorderColor() const
{
    if (m_borderGeometry.get())
    {
        osg::Vec4Array& colors = *static_cast<osg::Vec4Array*>(m_borderGeometry->getColorArray());
		osg::Vec4f color = colors.back();
		// Publicly visible color must have the original alpha, not the (possibly animated) current one
		color.a() = m_borderAlpha;
        return color;
    }
    return osg::Vec4f(0.0, 0.0, 0.0, 0.0);
}

void Widget::setInternalBorderColor(const osg::Vec4f& color)
{
    osg::Vec4Array& colors = *static_cast<osg::Vec4Array*>(m_borderGeometry->getColorArray());
    colors.back() = color;
}

void Widget::setBorderColor(const osg::Vec4f& color)
{
    if (!m_borderGeometry.get())
        return;
    setInternalBorderColor(color);
    m_borderAlpha = color.a();
}

void Widget::setImage(osg::Image* image)
{
    if (!image)
        return;

    osg::Texture* texture = new osg::Texture2D();
    texture->setImage(0, image);
    setTexture(texture);
    osg::Vec2f size(image->s(), image->t());
    setSize(size);
}

bool Widget::setImage(const std::string& filePath)
{
    if(!osgDB::findDataFile(filePath).size())
        return false;

    setImage(osgDB::readImageFile(filePath));
    return true;
}

void Widget::setHighlight(bool highlight)
{
	/*osg::Vec4Array* grey = new osg::Vec4Array;
	grey->push_back(osg::Vec4(0.5, 0.5, 0.5, 1.0));
	m_borderGeometry->setColorArray(grey);*/

	m_highlight = highlight;
	float value = (highlight ? 3.0f : 1.0f);

	/*osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));

	if (highlight==true)
		m_borderGeometry->setColorArray(colors);*/

	m_borderGeometry->getOrCreateStateSet()->setAttribute(new osg::LineWidth(value));
}

void Widget::setColors(osg::Vec4 color){
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(color);
	m_borderGeometry->setColorArray(colors);
}

void Widget::setTexture(osg::Texture* texture)
{
    m_bgGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
}


void Widget::setHandler(osgNewWidget::IWidgetEventHandler* handler)
{
    m_handler = handler;
}

void Widget::setZOffset(int zoffset)
{
    //OSG_ALWAYS << "Setting zoffset " << zoffset << std::endl;
    // TODO: Warn if widget wasn't added to a canvas - no effect in this case
    m_zoffset = zoffset;
    float z = zoffset * 0.01f;
    if (m_bgGeometry.get())
    {
        osg::Vec3Array& a = *static_cast<osg::Vec3Array*>(m_bgGeometry->getVertexArray());
        for (int i = 0; i < 4; i++)
            a[i].z() = z;
    }
    if (m_borderGeometry.get())
    {
        osg::Vec3Array& b = *static_cast<osg::Vec3Array*>(m_borderGeometry->getVertexArray());
        for (int i = 0; i < 4; i++)
            b[i].z() = z + 0.01f;
    }
}

void Widget::setParentOffset(const osg::Vec2f& offset)
{
    osg::Vec2f pos = getPosition();
    m_parentOffset = offset;
    setPosition(pos);
}
//
//float Widget::getAnimPropertyValue(AnimProperty animproperty)
//{
//    switch(animproperty)
//    {
//    case ANIM_PROPERTY_WIDTH:
//        return getSize().x();
//    case ANIM_PROPERTY_HEIGHT:
//        return getSize().y();
//    case ANIM_PROPERTY_POS_X:
//        return getPosition().x();
//    case ANIM_PROPERTY_POS_Y:
//        return getPosition().y();
//    case ANIM_PROPERTY_ALPHA_MULT:
//        return getAlphaMult();
//    }
//    return 0.0f;
//}
//
//void Widget::setAnimPropertyValue(AnimProperty animproperty, float value)
//{
//    osg::Vec2f size;
//    osg::Vec2f pos;
//    switch(animproperty)
//    {
//    case ANIM_PROPERTY_WIDTH:
//        size = getSize();
//        size.x() = floor(value);
//        setSize(size);
//        return;
//    case ANIM_PROPERTY_HEIGHT:
//        size = getSize();
//        size.y() = floor(value);
//        setSize(size);
//        return;
//    case ANIM_PROPERTY_POS_X:
//        pos = getPosition();
//        pos.x() = floor(value);
//        setPosition(pos);
//        return;
//    case ANIM_PROPERTY_POS_Y:
//        pos = getPosition();
//        pos.y() = floor(value);
//        setPosition(pos);
//        return;
//    case ANIM_PROPERTY_ALPHA_MULT:
//        setAlphaMult(value);
//        return;
//    }
//}

void Widget::setAlphaMult(float a)
{
    m_alphaMult = a;
    osg::Vec4f c;
    if (m_bgGeometry)
    {
        c = getColor();
        c.a() = m_alpha * m_alphaMult;
        setInternalGeometryColor(c);
    }
    if (m_borderGeometry)
    {
        c = getBorderColor();
        c.a() = m_borderAlpha * m_alphaMult;
        setInternalBorderColor(c);
    }
}

void Widget::show()
{
	setNodeMask(~0x0u);
}

void Widget::hide()
{
	setNodeMask(0u);
}

}
