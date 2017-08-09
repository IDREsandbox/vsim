#include <osgDB/ObjectWrapper>

#include "narrative/NarrativeGroup.h"

REGISTER_OBJECT_WRAPPER(
	NarrativeGroup,
	new ::NarrativeGroup,
	::NarrativeGroup,
	"osg::Object osg::Node osg::Group ::Group ::NarrativeGroup" )
{
}
