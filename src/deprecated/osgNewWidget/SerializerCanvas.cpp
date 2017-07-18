/*
 * SerializerCanvas.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>
#include "deprecated/osgNewWidget/Canvas.h"

REGISTER_OBJECT_WRAPPER( Canvas,
                         new osgNewWidget::Canvas,
                         osgNewWidget::Canvas,
                         "osg::Object osg::Node osg::Geode osgNewWidget::Widget osgNewWidget::Canvas" )
{
    ADD_OBJECT_SERIALIZER(Group, osg::Group, NULL);
}
