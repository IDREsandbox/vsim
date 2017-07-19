/*
 * SerializerVSCanvas.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eduardo
 */
#include <osgDB/ObjectWrapper>

#include "deprecated/osgNewWidget/VSWidget.h"

REGISTER_OBJECT_WRAPPER( VSCanvas,
                         new ::VSCanvas(osgNewWidget::WIDGET_NONE),
                         ::VSCanvas,
                         "osg::Object osg::Node osg::Geode osgNewWidget::Widget osgNewWidget::Canvas ::VSCanvas" )
{
}