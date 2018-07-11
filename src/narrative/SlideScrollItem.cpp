#include "SlideScrollItem.h"

#include <QPainter>

SlideScrollItem::SlideScrollItem(QWidget *parent)
	: ScrollBoxItem(parent),
	m_slide(nullptr)
{
	ui.setupUi(this);

	ui.transition_label->installEventFilter(this);
	ui.duration_label->installEventFilter(this);

	m_thumbnail_dirty = true;
}

void SlideScrollItem::setPixmap(QPixmap pixmap)
{
	ui.image_label->setPixmap(pixmap);
}

void SlideScrollItem::setTransition(float duration)
{
	ui.transition_label->setText(QString::number(duration, 'f', 1) + "s");
}

void SlideScrollItem::setDuration(bool stay, float duration)
{
	if (stay) {
		ui.duration_label->setText("-");
	}
	else {
		ui.duration_label->setText(QString::number(duration, 'f', 1) + "s");
	}
}

void SlideScrollItem::setIndex(int index)
{
	ScrollBoxItem::setIndex(index);
	ui.number_label->setText(QString::number(index+1));
	if (index == 0) {
		ui.transition_widget->hide();
	}
	else {
		ui.transition_widget->show();
	}
}

int SlideScrollItem::widthFromHeight(int height) const
{
	//return 2*height;
	// Force a 16x9 after subtracting out the label sizes
	QMargins slide_margin = ui.slide_layout->contentsMargins();
	int extra_height = ui.number_label->minimumSizeHint().height() + ui.duration_label->minimumSizeHint().height()
		+ slide_margin.top() + slide_margin.bottom() + 2 * ui.slide_layout->spacing();

	int image_height = height - extra_height;
	int image_width = (16.0 / 9.0)*image_height;

	QMargins transition_margin = ui.transition_layout->contentsMargins();

	int extra_width = ui.transition_widget->minimumSizeHint().width()
		+ ui.horizontal_layout->spacing();

	if (getIndex() == 0) {
		return image_width; // just for the first item where there is no transition time
	}
	return image_width + extra_width;
}

bool SlideScrollItem::eventFilter(QObject * obj, QEvent * event)
{	
	if (obj == ui.transition_label) {
		if (event->type() == QEvent::MouseButtonDblClick) {
			emit sTransitionDoubleClick();
			return false;
		}
	}
	if (obj == ui.duration_label) {
		if (event->type() == QEvent::MouseButtonDblClick) {
			emit sDurationDoubleClick();
			return false;
		}
	}
	//if (obj == ui.transition_widget) {
	//	if (event->type() == QEvent::MouseButtonPress) {
	//		emit sTransitionClick();
	//		return false;
	//	}	
	//}
	return false;
}

void SlideScrollItem::setSlide(NarrativeSlide *slide)
{
	if (m_slide != nullptr) {
		disconnect(m_slide, 0, this, 0);
	}

	m_slide = slide;

	if (slide == nullptr) return;

	setDuration(slide->getStayOnNode(), slide->getDuration());
	setTransition(slide->getTransitionDuration());
	setPixmap(slide->thumbnail());

	connect(slide, &NarrativeSlide::sStayOnNodeChanged, this, [this](bool stay) {setDuration(m_slide->getStayOnNode(), m_slide->getDuration());});
	connect(slide, &NarrativeSlide::sDurationChanged, this, [this](float duration) {setDuration(m_slide->getStayOnNode(), m_slide->getDuration());});
	connect(slide, &NarrativeSlide::sTransitionDurationChanged, this, &SlideScrollItem::setTransition);
	connect(slide, &NarrativeSlide::sThumbnailChanged, this, &SlideScrollItem::setPixmap);
}

NarrativeSlide *SlideScrollItem::getSlide()
{
	return m_slide;
}
