#include "SlideScrollItem.h"

SlideScrollItem::SlideScrollItem() 
	: ScrollBoxItem() 
{
	ui.setupUi(this);

	ui.transition_label->installEventFilter(this);
	ui.duration_label->installEventFilter(this);

	setTransition(2.0f);
	setDuration(0.0f);
}

void SlideScrollItem::setImage(const QImage & img)
{
	//ui.image_label->set
	ui.image_label->setPixmap(QPixmap::fromImage(img));
}

float SlideScrollItem::getTransition()
{
	return m_transition_duration;
}
float SlideScrollItem::getDuration()
{
	return m_duration;
}
void SlideScrollItem::setTransition(float duration)
{
	m_transition_duration = duration;
	ui.transition_label->setText(QString::number(duration) + "s");
}

void SlideScrollItem::setDuration(float duration)
{
	m_duration = duration;
	if (m_duration <= 0) {
		ui.duration_label->setText("-");
	}
	else {
		ui.duration_label->setText(QString::number(duration) + "s");
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

int SlideScrollItem::widthFromHeight(int height)
{
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

void SlideScrollItem::colorFocus(bool color)
{
}

void SlideScrollItem::colorSelect(bool color)
{
	if (color) {
		this->setStyleSheet("background-color: rgba(0,0,255,50);");
	}
	else {
		this->setStyleSheet("background-color: rgba(0,0,0,0);");
	}
}

bool SlideScrollItem::eventFilter(QObject * obj, QEvent * event)
{	
	if (obj == ui.transition_label) {
		if (event->type() == QEvent::MouseButtonDblClick) {
			qDebug() << "slide item transition - double click";
			emit sTransitionDoubleClick();
			return false;
		}
	}
	if (obj == ui.duration_label) {
		if (event->type() == QEvent::MouseButtonDblClick) {
			qDebug() << "slide item duration - double click";
			emit sDurationDoubleClick();
			return false;
		}
	}
	return false;
}


