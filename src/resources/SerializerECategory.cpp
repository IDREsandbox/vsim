#include <osgDB/ObjectWrapper>

#include "resources/ECategory.h"

REGISTER_OBJECT_WRAPPER(
	ECategory,
	new ::ECategory,
	::ECategory,
	"osg::Object osg::Node osg::Group ::Group ::ECategory")
{
	ADD_STRING_SERIALIZER(CategoryName, "");
	ADD_INT_SERIALIZER(Blue, 0);
	ADD_INT_SERIALIZER(Green, 0);
	ADD_INT_SERIALIZER(Red, 0);
}
