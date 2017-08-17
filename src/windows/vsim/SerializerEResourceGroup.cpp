#include <osgDB/ObjectWrapper>

#include "EResourceGroup.h"

REGISTER_OBJECT_WRAPPER(
	EResourceGroup,
	new ::EResourceGroup,
	::EResourceGroup,
	"osg::Object osg::Node osg::Group ::Group ::EResourceGroup")
{
}
