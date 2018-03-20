#include <osgDB/ObjectWrapper>

#include "narrative/NarrativeSlideLabel.h"

REGISTER_OBJECT_WRAPPER(
	NarrativeSlideLabel,
	new ::NarrativeSlideLabel,
	::NarrativeSlideLabel,
	"osg::Object osg::Node ::NarrativeSlideItem ::NarrativeSlideLabel")
{
	ADD_STRING_SERIALIZER(Html, "New Label");
	ADD_INT_SERIALIZER(StyleTypeInt, 0);
	ADD_INT_SERIALIZER(VAlign, 0);
}