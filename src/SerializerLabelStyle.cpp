#include <osgDB/ObjectWrapper>

#include "LabelStyle.h"

REGISTER_OBJECT_WRAPPER(
	LabelStyle,
	new ::LabelStyle,
	::LabelStyle,
	"osg::Object osg::Node osg::Group ::Group ::LabelStyle")
{
	ADD_STRING_SERIALIZER(Font, "\"Arial\"");
	ADD_INT_SERIALIZER(Red, 255);
	ADD_INT_SERIALIZER(Blue, 255);
	ADD_INT_SERIALIZER(Green, 255);
	ADD_INT_SERIALIZER(Size, 18);
	ADD_FLOAT_SERIALIZER(Opacity, 100);
	ADD_BOOL_SERIALIZER(Shadow, false);

	ADD_INT_SERIALIZER(Red_BG, 0);
	ADD_INT_SERIALIZER(Blue_BG, 0);
	ADD_INT_SERIALIZER(Green_BG, 0);
	ADD_FLOAT_SERIALIZER(Opacity_BG, 70);
	ADD_INT_SERIALIZER(Width, 200);
	ADD_INT_SERIALIZER(Height, 150);

	ADD_STRING_SERIALIZER(Weight, "regular");
	ADD_STRING_SERIALIZER(Align, "left");
}
