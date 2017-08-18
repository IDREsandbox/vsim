#include <osgDB/ObjectWrapper>

#include "ModelData.h"

REGISTER_OBJECT_WRAPPER(
	ModelData,
	new ::ModelData,
	::ModelData,
	"osg::Object osg::Node osg::Group ::ModelData")
{
	ADD_INT_SERIALIZER(YearBegin, 0);
	ADD_INT_SERIALIZER(YearEnd, 0);
}
