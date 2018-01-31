#include <osgDB/ObjectWrapper>

#include "narrative/NarrativeSlideItem.h"

REGISTER_OBJECT_WRAPPER( 
	NarrativeSlideItem,
	new ::NarrativeSlideItem,
	::NarrativeSlideItem,
	"osg::Object osg::Node ::NarrativeSlideItem" )
{
	ADD_VEC4_SERIALIZER(XYWH, osg::Vec4(-.1, -.05, .2, .1));
	ADD_VEC4_SERIALIZER(BackgroundRGBA, osg::Vec4(0, 0, 0, 0));
}
