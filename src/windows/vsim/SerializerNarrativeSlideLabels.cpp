#include <osgDB/ObjectWrapper>

#include "NarrativeSlideLabels.h"

REGISTER_OBJECT_WRAPPER(
	NarrativeSlideLabels,
	new ::NarrativeSlideLabels,
	::NarrativeSlideLabels,
	"osg::Object osg::Node ::NarrativeSlideLabels")
{
	ADD_INT_SERIALIZER(X, 250);
	ADD_INT_SERIALIZER(Y, 250);
	ADD_INT_SERIALIZER(W, 250);
	ADD_INT_SERIALIZER(H, 150);
	ADD_INT_SERIALIZER(ParW, 800);
	ADD_INT_SERIALIZER(ParH, 600);
	ADD_FLOAT_SERIALIZER(RatW, .5);
	ADD_FLOAT_SERIALIZER(RatH, .5);
	ADD_STRING_SERIALIZER(Text, "New Label");
	ADD_STRING_SERIALIZER(Style, "background: rgba(0, 0, 0, 70); color: rgb(255, 255, 255);");
}