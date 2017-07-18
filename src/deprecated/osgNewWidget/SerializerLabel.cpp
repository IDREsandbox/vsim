/*
 * SerializerLabel.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>
#include "deprecated/osgNewWidget/Label.h"

REGISTER_OBJECT_WRAPPER( Label,
                         new osgNewWidget::Label,
                         osgNewWidget::Label,
                         "osg::Object osg::Node osg::Geode osgNewWidget::Widget osgNewWidget::Label" )
{
    ADD_OBJECT_SERIALIZER(Text, osgText::Text, NULL);
}
