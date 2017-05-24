/*
 * SerializerNarrativeReference.cpp
 *
 *  Created on: Mar 29, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

#include "narrative/NarrativeReference.h"

REGISTER_OBJECT_WRAPPER( NarrativeReference,
                         new ::NarrativeReference,
                         ::NarrativeReference,
                         "osg::Object osg::Node ::NarrativeReference" )
{
    ADD_STRING_SERIALIZER(Filename, "");
}
