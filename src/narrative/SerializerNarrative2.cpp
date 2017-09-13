#include <osgDB/ObjectWrapper>

#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"
#include "LabelStyle.h"

REGISTER_OBJECT_WRAPPER( 
	Narrative2,
	new ::Narrative2,
	::Narrative2,
	"osg::Object osg::Node osg::Group ::Group ::Narrative2" )
{
	ADD_STRING_SERIALIZER(Title, "Untitled");
	ADD_STRING_SERIALIZER(Author, "");
	ADD_STRING_SERIALIZER(Description, "");
	ADD_BOOL_SERIALIZER(Lock, false);

	ADD_OBJECT_SERIALIZER(H1, ::LabelStyle, nullptr);
	ADD_OBJECT_SERIALIZER(H2, ::LabelStyle, nullptr);
	ADD_OBJECT_SERIALIZER(Bod, ::LabelStyle, nullptr);
	ADD_OBJECT_SERIALIZER(Lab, ::LabelStyle, nullptr);
	ADD_OBJECT_SERIALIZER(Img, ::LabelStyle, nullptr);
}
