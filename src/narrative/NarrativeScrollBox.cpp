#include "NarrativeScrollBox.h"
#include "NarrativeScrollItem.h"

NarrativeScrollBox::NarrativeScrollBox(QWidget * parent)
	: GroupScrollBox<Narrative>(parent)
{
	setMIMEType("application/x-narrative");
}

ScrollBoxItem * NarrativeScrollBox::createItem(Narrative * narrative)
{
	NarrativeScrollItem *item = new NarrativeScrollItem(this);
	item->setNarrative(narrative);
	return item;
}

void NarrativeScrollBox::itemMouseDoubleClickEvent(QMouseEvent * event, int index)
{
	emit sOpen();
}

static int i = 0;
void NarrativeScrollBox::paintEvent(QPaintEvent * e)
{
	qDebug() << "nar paint event" << i;
	i++;
	HorizontalScrollBox::paintEvent(e);
}
