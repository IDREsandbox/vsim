/*
 * SerializerWidget.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>

#include "deprecated/osgNewWidget/VSWidget.h"

REGISTER_OBJECT_WRAPPER( VSWidget,
                         new ::VSWidget(osgNewWidget::WIDGET_NONE),
                         ::VSWidget,
                         "osg::Object osg::Node osg::Geode osgNewWidget::Widget ::VSWidget" )
{
}
