#include <osgDB/ObjectWrapper>

#include "narrative/NarrativeSlideLabel.h"

REGISTER_OBJECT_WRAPPER(
	NarrativeSlideLabel,
	new ::NarrativeSlideLabel,
	::NarrativeSlideLabel,
	"osg::Object osg::Node ::NarrativeSlideItem ::NarrativeSlideLabel")
{
	ADD_INT_SERIALIZER(Style, 0);
	ADD_STRING_SERIALIZER(Html, "New Label");
}