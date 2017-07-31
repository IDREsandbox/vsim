#include <osgDB/ObjectWrapper>

#include "narrative/NarrativeSlideLabels.h"

REGISTER_OBJECT_WRAPPER(
	NarrativeSlideLabels,
	new ::NarrativeSlideLabels,
	::NarrativeSlideLabels,
	"osg::Object osg::Node ::NarrativeSlideLabels")
{
	ADD_FLOAT_SERIALIZER(rX, 0.5);
	ADD_FLOAT_SERIALIZER(rY, 0.5);
	ADD_FLOAT_SERIALIZER(rW, .25);
	ADD_FLOAT_SERIALIZER(rH, .20);
	ADD_STRING_SERIALIZER(Text, "New Label");
	ADD_STRING_SERIALIZER(Style, "background: rgba(0, 0, 0, 70); color: rgb(255, 255, 255);");
}