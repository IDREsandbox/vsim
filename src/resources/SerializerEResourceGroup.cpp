#include <osgDB/ObjectWrapper>

#include "resources/EResourceGroup.h"
#include "resources/ECategoryGroup.h"

REGISTER_OBJECT_WRAPPER(
	EResourceGroup,
	new ::EResourceGroup,
	::EResourceGroup,
	"osg::Object osg::Node osg::Group ::Group ::EResourceGroup")
{
	ADD_OBJECT_SERIALIZER(Categories, ::ECategoryGroup, nullptr);
}
