#include <osgDB/ObjectWrapper>

#include "ModelGroup.h"

REGISTER_OBJECT_WRAPPER(
	ModelGroup,
	new ::ModelGroup,
	::ModelGroup,
	"osg::Object osg::Node osg::Group ::Group ::ModelGroup")
{
	ADD_OBJECT_SERIALIZER(DataTable, ::ModelDataTable, nullptr);
}
