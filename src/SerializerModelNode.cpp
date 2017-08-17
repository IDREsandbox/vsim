#include <osgDB/ObjectWrapper>

#include "ModelNode.h"

REGISTER_OBJECT_WRAPPER(
	ModelNode,
	new ::ModelNode,
	::ModelNode,
	"osg::Object osg::Node osg::Group")
{
	ADD_INT_SERIALIZER(YearBegin, 0);
	ADD_INT_SERIALIZER(YearEnd, 0);
}
