#include "resources/ERScrollBox.h"
#include "ERFilterSortProxy.h"
#include "resources/ERScrollItem.h"

ERScrollBox::ERScrollBox(QWidget * parent)
	: GroupScrollBox(parent)
{
	enableDragging(false);
}

ScrollBoxItem * ERScrollBox::createItem(osg::Node * node)
{
	EResource *er = dynamic_cast<EResource*>(node);
	if (er == nullptr) {
		qWarning() << "ER scroll box insert new ER. Node" << node << "is not a ER, creating a null item anyway";
		//return nullptr;
	}
	ERScrollItem *item = new ERScrollItem(this);
	item->setER(er);
	return item;
}

void ERScrollBox::itemMouseDoubleClickEvent(QMouseEvent * event, int index)
{
	emit sOpen();
}