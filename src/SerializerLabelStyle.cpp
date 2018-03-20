#include <osgDB/ObjectWrapper>

#include "LabelStyle.h"
#include "LabelType.h"
REGISTER_OBJECT_WRAPPER(
	LabelStyle,
	new ::LabelStyle,
	::LabelStyle,
	"osg::Object osg::Node ::LabelStyle")
{
	ADD_INT_SERIALIZER(TypeInt, LabelType::NONE);

	ADD_VEC4_SERIALIZER(BackgroundColor, osg::Vec4());
	ADD_VEC4_SERIALIZER(ForegroundColor, osg::Vec4());
	ADD_INT_SERIALIZER(Align, 0);
	ADD_INT_SERIALIZER(Margin, 0);

	ADD_STRING_SERIALIZER(FontFamily, "");
	ADD_INT_SERIALIZER(PointSize, 12);
	ADD_INT_SERIALIZER(Weight, 50);
	ADD_BOOL_SERIALIZER(Italicized, false);
	ADD_BOOL_SERIALIZER(Underline, false);
}
