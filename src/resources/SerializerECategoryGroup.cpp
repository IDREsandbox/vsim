#include <osgDB/ObjectWrapper>

#include "resources/ECategoryGroup.h"

REGISTER_OBJECT_WRAPPER(
	ECategoryGroup,
	new ::ECategoryGroup,
	::ECategoryGroup,
	"osg::Object osg::Node osg::Group ::Group ::ECategoryGroup")
{
}
