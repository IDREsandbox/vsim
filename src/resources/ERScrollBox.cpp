#include "resources/ERScrollBox.h"
#include "ERFilterSortProxy.h"
#include "resources/ERScrollItem.h"

ERScrollBox::ERScrollBox(QWidget * parent)
	: GroupScrollBox(parent)
{
	enableDragging(false);
}

ScrollBoxItem * ERScrollBox::createItem(EResource *er)
{
	ERScrollItem *item = new ERScrollItem(this);
	item->setER(er);
	return item;
}

void ERScrollBox::itemMouseDoubleClickEvent(QMouseEvent * event, int index)
{
	emit sOpen();
}