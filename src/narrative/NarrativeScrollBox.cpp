#include "NarrativeScrollBox.h"
#include "NarrativeScrollItem.h"

NarrativeScrollBox::NarrativeScrollBox(QWidget * parent)
	: GroupScrollBox(parent)
{
	setMIMEType("application/x-narrative");
}

ScrollBoxItem * NarrativeScrollBox::createItem(osg::Node * node)
{
	Narrative *narrative = dynamic_cast<Narrative*>(node);
	if (narrative == nullptr) {
		qWarning() << "Narrative scroll box insert new narrative. Node" << node << "is not a NarrativeSlide, creating a null item anyway";
		//return nullptr;
	}
	NarrativeScrollItem *item = new NarrativeScrollItem(this);
	item->setNarrative(narrative);
	return item;
}

void NarrativeScrollBox::itemMouseDoubleClickEvent(QMouseEvent * event, int index)
{
	emit sOpen();
}
