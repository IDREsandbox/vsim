#include <osgDB/ObjectWrapper>

#include "VSimRoot.h"
#include "narrative/NarrativeGroup.h"
#include "ModelGroup.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategoryGroup.h"

REGISTER_OBJECT_WRAPPER(
	VSimRoot,
	new ::VSimRoot,
	::VSimRoot,
	"osg::Object osg::Node osg::Group ::VSimRoot")
{
	ADD_OBJECT_SERIALIZER(Narratives, ::NarrativeGroup, nullptr);
	ADD_OBJECT_SERIALIZER(Models, ::ModelGroup, nullptr);
	ADD_OBJECT_SERIALIZER(Resources, ::EResourceGroup, nullptr);
	ADD_OBJECT_SERIALIZER(Categories, ::ECategoryGroup, nullptr);
}
