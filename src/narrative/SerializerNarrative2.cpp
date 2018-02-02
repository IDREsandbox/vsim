#include <osgDB/ObjectWrapper>

#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"
#include "LabelStyle.h"
#include "LabelStyleGroup.h"

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

	ADD_OBJECT_SERIALIZER(LabelStyles, ::LabelStyleGroup, nullptr);
}
