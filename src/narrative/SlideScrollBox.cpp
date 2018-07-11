#include "SlideScrollBox.h"
#include "SlideScrollItem.h"
#include "narrative/NarrativeSlide.h"
#include "Selection.h"

SlideScrollBox::SlideScrollBox(QWidget * parent) 
	: GroupScrollBox(parent)
{
	setSpacing(10);
	setMIMEType("application/x-slide");
}

ScrollBoxItem * SlideScrollBox::createItem(NarrativeSlide *slide)
{
	SlideScrollItem *item = new SlideScrollItem(this);
	item->setSlide(slide);
	connect(item, &SlideScrollItem::sDurationDoubleClick, this, &SlideScrollBox::sSetDuration);
	connect(item, &SlideScrollItem::sTransitionDoubleClick, this, &SlideScrollBox::sSetTransitionDuration);
	//connect(item, &SlideScrollItem::sTransitionClick, this, [this]() {
	//	auto item = dynamic_cast<SlideScrollItem*>(QObject::sender());
	//	emit sTransitionTo(item->getIndex());
	//});

	return item;
}

void SlideScrollBox::paintEvent(QPaintEvent * event)
{
	//auto dirty = getDirtySlides();
	//if (!dirty.empty()) {
	//	emit sThumbnailsDirty();
	//}
	HorizontalScrollBox::paintEvent(event);
}

void SlideScrollBox::itemMousePressEvent(QMouseEvent * event, int index)
{
	HorizontalScrollBox::itemMousePressEvent(event, index);
	if (event->button() == Qt::LeftButton && m_selection->contains(index)) {
		emit sGoto(index);
	}
}


//connect(new_item->getSlideWidget(), &SlideScrollWidget::sMousePressEvent, this,
//	[this](QMouseEvent* e) {
//	qDebug() << "slide mouse press event";
//
//	if (e->button() == Qt::RightButton) {
//		m_slide_menu->exec(e->globalPos());
//	}
//
//});