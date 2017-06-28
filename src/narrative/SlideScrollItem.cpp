#include "SlideScrollItem.h"

SlideScrollItem::SlideScrollItem() : ScrollBoxItem() {
	ui.setupUi(this);

	//ui.transition_widget->setFixedWidth(26);
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
		this->setStyleSheet("background-color: rgba(255,255,255,10);");
	}
	else {
		this->setStyleSheet("background-color: rgba(0,0,255,0);");
	}
}

