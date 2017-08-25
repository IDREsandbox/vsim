#include <osgDB/ObjectWrapper>

#include "resources/EResourceGroup.h"

REGISTER_OBJECT_WRAPPER(
	EResourceGroup,
	new ::EResourceGroup,
	::EResourceGroup,
	"osg::Object osg::Node osg::Group ::Group ::EResourceGroup")
{
}
