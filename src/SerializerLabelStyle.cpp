#include <osgDB/ObjectWrapper>

#include "LabelStyle.h"

REGISTER_OBJECT_WRAPPER(
	LabelStyle,
	new ::LabelStyle,
	::LabelStyle,
	"osg::Object osg::Node ::LabelStyle")
{
	ADD_STRING_SERIALIZER(Html, "");
	ADD_INT_SERIALIZER(BaseStyle, 0);

	ADD_VEC4_SERIALIZER(BackgroundColor, osg::Vec4());
	//ADD_INT_SERIALIZER(Width, 200);
	//ADD_INT_SERIALIZER(Height, 150);

}
