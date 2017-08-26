/*
 * SeriallizerEResourcesNode.cpp
 *
 *  Created on: January 14, 2011
 *      Author: Franklin Fang
 */

#include "test_Prefix.pch"


#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

#include "resources/EResourcesNode.h"
#include "resources/EResourcesCategory.h"


REGISTER_OBJECT_WRAPPER( EResourcesNode,
                         new ::EResourcesNode,
                         ::EResourcesNode,
                         "osg::Object osg::Node osg::Group ::EResourcesNode" )
{
	ADD_MATRIXD_SERIALIZER(ViewMatrix, osg::Matrixd());

    ADD_STRING_SERIALIZER(ResourceName, "");
	ADD_STRING_SERIALIZER(ResourceType, "");
	ADD_STRING_SERIALIZER(ResourcePath, "");
	ADD_STRING_SERIALIZER(Author, "");
	ADD_STRING_SERIALIZER(ResourceDiscription, "");
	ADD_INT_SERIALIZER(Global, 1);
	ADD_INT_SERIALIZER(MinYear, -9999);
	ADD_INT_SERIALIZER(MaxYear, 9999);
	ADD_INT_SERIALIZER(Reposition, 1);
	ADD_INT_SERIALIZER(AutoLaunch, 1);
	ADD_INT_SERIALIZER(CopyRight, 0);
	ADD_FLOAT_SERIALIZER(LocalRange, 10.0f);
	ADD_INT_SERIALIZER(ErType, 0);

	ADD_STRING_SERIALIZER(CategoryName, "");
	ADD_UINT_SERIALIZER(Red,0);
	ADD_UINT_SERIALIZER(Green,0);
	ADD_UINT_SERIALIZER(Blue,0);
}