/*
 * SerializerWidget.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>

#include "deprecated/osgNewWidget/Widget.h"
#include <osg/Geometry>

REGISTER_OBJECT_WRAPPER( Widget,
                         new osgNewWidget::Widget(osgNewWidget::WIDGET_NONE),
                         osgNewWidget::Widget,
                         "osg::Object osg::Node osg::Geode osgNewWidget::Widget" )
{
    ADD_VEC2F_SERIALIZER(Position, osg::Vec2f());
    ADD_VEC2F_SERIALIZER(Size, osg::Vec2f());
    ADD_OBJECT_SERIALIZER(BgGeometry, osg::Geometry, NULL);
    ADD_OBJECT_SERIALIZER(BorderGeometry, osg::Geometry, NULL);
    ADD_INT_SERIALIZER(ZOffset, 0);
    ADD_VEC2F_SERIALIZER(ParentOffset, osg::Vec2f());
    BEGIN_ENUM_SERIALIZER4( osgNewWidget, AnchorHEdge, ANCHOR_L );
        ADD_ENUM_CLASS_VALUE( osgNewWidget, ANCHOR_L );
        ADD_ENUM_CLASS_VALUE( osgNewWidget, ANCHOR_LR );
        ADD_ENUM_CLASS_VALUE( osgNewWidget, ANCHOR_R );
    END_ENUM_SERIALIZER();
    BEGIN_ENUM_SERIALIZER4( osgNewWidget, AnchorVEdge, ANCHOR_B );
        ADD_ENUM_CLASS_VALUE( osgNewWidget, ANCHOR_B );
        ADD_ENUM_CLASS_VALUE( osgNewWidget, ANCHOR_BT );
        ADD_ENUM_CLASS_VALUE( osgNewWidget, ANCHOR_T );
    END_ENUM_SERIALIZER();
    ADD_VEC2F_SERIALIZER(AnchorValue, osg::Vec2f());
    //ADD_OBJECT_SERIALIZER(Parent, osgNewWidget::Widget, NULL);
}
