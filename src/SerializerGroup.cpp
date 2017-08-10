#include <osgDB/ObjectWrapper>

#include "Group.h"

REGISTER_OBJECT_WRAPPER(
	Group,
	new ::Group,
	::Group,
	"osg::Object osg::Node osg::Group")
{
}
