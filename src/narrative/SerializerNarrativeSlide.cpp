#include <osgDB/ObjectWrapper>

#include "narrative/NarrativeSlide.h"

REGISTER_OBJECT_WRAPPER( 
	NarrativeSlide,
	new ::NarrativeSlide,
	::NarrativeSlide,
	"osg::Object osg::Node osg::Group ::NarrativeSlide" )
{
	ADD_MATRIXD_SERIALIZER(CameraMatrix, osg::Matrixd());
	ADD_FLOAT_SERIALIZER(Duration, 15.0f);
	ADD_BOOL_SERIALIZER(StayOnNode, false);
	ADD_FLOAT_SERIALIZER(TransitionDuration, 4.0f);
	//ADD_IMAGE_SERIALIZER(Thumbnail, osg::Image, NULL);
}
