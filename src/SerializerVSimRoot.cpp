#include <osgDB/ObjectWrapper>

#include "VSimRoot.h"

REGISTER_OBJECT_WRAPPER(
	VSimRoot,
	new ::VSimRoot,
	::VSimRoot,
	"osg::Object osg::Node osg::Group ::VSimRoot")
{
	ADD_OBJECT_SERIALIZER(Narratives, ::NarrativeGroup, nullptr);
	ADD_OBJECT_SERIALIZER(Models, ::ModelGroup, nullptr);
}
