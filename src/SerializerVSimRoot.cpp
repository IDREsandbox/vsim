#include <osgDB/ObjectWrapper>

#include "VSimRoot.h"

REGISTER_OBJECT_WRAPPER(
	VSimRoot,
	new ::VSimRoot,
	::VSimRoot,
	"osg::Object osg::Node osg::Group ::VSimRoot")
{
	//ADD_OBJECT_SERIALIZER(Narratives, ::NarrativeGroup, nullptr);
	//ADD_OBJECT_SERIALIZER(Models, ::ModelGroup, nullptr);
	ADD_INT_SERIALIZER(Foo, 3);
	ADD_INT_SERIALIZER(Foob, 2);
	ADD_INT_SERIALIZER(Foox, 1);
	//ADD_OBJECT_SERIALIZER(Models, ::ModelGroup, nullptr);
	//ADD_OBJECT_SERIALIZER(Resources, ::EResourceGroup, nullptr);
	//ADD_OBJECT_SERIALIZER(Categories, ::ECategoryGroup, nullptr);
}
