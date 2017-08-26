#include <osgDB/ObjectWrapper>

#include "resources/EResource.h"

REGISTER_OBJECT_WRAPPER(
	EResource,
	new ::EResource,
	::EResource,
	"osg::Object osg::Node osg::Group ::Group ::EResource")
{
	ADD_STRING_SERIALIZER(ResourceName, "Untitled");
	ADD_STRING_SERIALIZER(Author, "");
	ADD_STRING_SERIALIZER(ResourceType, "");
	ADD_STRING_SERIALIZER(ResourcePath, "");
	ADD_STRING_SERIALIZER(ResourceDescription, "");

	ADD_STRING_SERIALIZER(CategoryName, "");
	ADD_INT_SERIALIZER(Blue, 0);
	ADD_INT_SERIALIZER(Green, 0);
	ADD_INT_SERIALIZER(Red, 0);

	ADD_INT_SERIALIZER(Global, 0);
	ADD_INT_SERIALIZER(CopyRight, 0);
	ADD_INT_SERIALIZER(MinYear, 0);
	ADD_INT_SERIALIZER(MaxYear, 0);
	ADD_INT_SERIALIZER(Reposition, 0);
	ADD_INT_SERIALIZER(AutoLaunch, 0);
	ADD_INT_SERIALIZER(ErType, 0);

	ADD_FLOAT_SERIALIZER(LocalRange, .20);

	ADD_MATRIXD_SERIALIZER(ViewMatrix, osg::Matrixd());
}
