#include <osgDB/ObjectWrapper>

#include "narrative/NarrativeSlideLabel.h"

REGISTER_OBJECT_WRAPPER(
	NarrativeSlideLabel,
	new ::NarrativeSlideLabel,
	::NarrativeSlideLabel,
	"osg::Object osg::Node ::NarrativeSlideLabel")
{
	ADD_FLOAT_SERIALIZER(rX, 0.5);
	ADD_FLOAT_SERIALIZER(rY, 0.5);
	ADD_FLOAT_SERIALIZER(rW, .25);
	ADD_FLOAT_SERIALIZER(rH, .20);
	ADD_INT_SERIALIZER(Margin, 10);
	ADD_STRING_SERIALIZER(Text, "New Label");
	ADD_STRING_SERIALIZER(Style, "p {color: rgb(255, 255, 255);}");
	ADD_STRING_SERIALIZER(WidgetStyle, "background: rgba(0, 0, 0, 70);");
	ADD_INT_SERIALIZER(Fuzz, 777);
}