/*
 * SerializerVSCanvas.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>

#include "osgNewWidget/VSWidget.h"

REGISTER_OBJECT_WRAPPER( VSLabel,
                         new ::VSLabel("", osgNewWidget::WIDGET_NONE),
                         ::VSLabel,
                         "osg::Object osg::Node osg::Geode osgNewWidget::Widget osgNewWidget::Label ::VSLabel" )
{
}
