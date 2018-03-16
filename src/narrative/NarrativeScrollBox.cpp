#include "NarrativeScrollBox.h"
#include "NarrativeScrollItem.h"

NarrativeScrollBox::NarrativeScrollBox(QWidget * parent)
	: GroupScrollBox(parent)
{
	setMIMEType("application/x-narrative");
}

ScrollBoxItem * NarrativeScrollBox::createItem(osg::Node * node)
{
	Narrative2 *narrative = dynamic_cast<Narrative2*>(node);
	if (narrative == nullptr) {
		qWarning() << "Narrative scroll box insert new narrative. Node" << node << "is not a NarrativeSlide, creating a null item anyway";
		//return nullptr;
	}
	NarrativeScrollItem *item = new NarrativeScrollItem(this);
	item->setNarrative(narrative);
	connect(item, &NarrativeScrollItem::sDoubleClick, this, &NarrativeScrollBox::sOpen);
	return item;
}

void NarrativeScrollBox::itemMouseDoubleClickEvent(QMouseEvent * event, int index)
{
	emit sOpen();
}
