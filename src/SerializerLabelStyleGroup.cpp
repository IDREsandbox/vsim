#include <osgDB/ObjectWrapper>

#include "LabelStyleGroup.h"

REGISTER_OBJECT_WRAPPER(
	LabelStyleGroup,
	new ::LabelStyleGroup,
	::LabelStyleGroup,
	"osg::Object osg::Node osg::Group ::Group ::LabelStyleGroup")
{
}
