/*
 * SerializerNarrativeTransition.cpp
 *
 *  Created on: Mar 26, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

#include "deprecated/narrative/NarrativeNode.h"

REGISTER_OBJECT_WRAPPER( NarrativeTransition,
                         new ::NarrativeTransition,
                         ::NarrativeTransition,
                         "osg::Object osg::Node ::NarrativeTransition" )
{
    ADD_FLOAT_SERIALIZER(Duration, 4.0f);
}
