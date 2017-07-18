/*
 * SerializerNarrative.cpp
 *
 *  Created on: Mar 26, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"

REGISTER_OBJECT_WRAPPER( Narrative2,
                         new ::Narrative2,
                         ::Narrative2,
                         "osg::Object osg::Node osg::Group ::Narrative2" )
{
	ADD_STRING_SERIALIZER(Name, "");
	ADD_STRING_SERIALIZER(Author, "");
	ADD_STRING_SERIALIZER(Description, "");
	ADD_BOOL_SERIALIZER(Lock, false);
}
