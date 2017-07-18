/*
 * SerializerNarrativeNode.cpp
 *
 *  Created on: Mar 26, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>

#include "narrative/NarrativeNode.h"

REGISTER_OBJECT_WRAPPER( NarrativeNode,
                         new ::NarrativeNode,
                         ::NarrativeNode,
                         "osg::Object osg::Node osg::Group ::NarrativeNode" )
{
    ADD_MATRIXD_SERIALIZER(ViewMatrix, osg::Matrixd());
    ADD_FLOAT_SERIALIZER(PauseAtNode, 15.0f);
	ADD_BOOL_SERIALIZER(StayOnNode, false);
	ADD_IMAGE_SERIALIZER(Image, osg::Image, NULL);
	//ADD_FLOAT_SERIALIZER(TransitionDuration, 4.0f);
	
}
