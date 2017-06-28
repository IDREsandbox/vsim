#include "SlideScrollItem.h"

SlideScrollItem::SlideScrollItem() : ScrollBoxItem() {
	ui.setupUi(this);

	ui.transition_widget->setFixedWidth(26);
}

void SlideScrollItem::setIndex(int index)
{
	ScrollBoxItem::setIndex(index);
	ui.number_label->setText(QString::number(index+1));
}

int SlideScrollItem::widthFromHeight(int height)
{
	QMargins slide_margin = ui.slide_layout->contentsMargins();
	int extra_height = ui.number_label->fontMetrics().height() + ui.duration_label->fontMetrics().height()
		+ slide_margin.top() + slide_margin.bottom() + 2 * ui.slide_layout->spacing();


	int image_height = height - extra_height;
	int image_width = (16.0 / 9.0)*image_height;
	
	int extra_width = ui.horizontal_layout->spacing() + ui.transition_widget->width();
	qDebug() << "transtition widgete wtieh" << ui.transition_widget->width();
	qDebug() << height << extra_height << image_height << image_width << extra_width;
	qDebug() << "heights" << ui.number_label->height() << ui.duration_label->height();
	qDebug() << "metricheight" << ui.number_label->fontMetrics().height() << ui.duration_label->fontMetrics().height();
	//qDebug() << "textstuff" << ui.duration_label->maximumHeight() << ui.duration_label->minimumHeight();
	return image_width + extra_width;
}

void SlideScrollItem::colorFocus(bool color)
{
}

void SlideScrollItem::colorSelect(bool color)
{
}

void SlideScrollItem::resizeEvent(QResizeEvent * event)
{
	//qDebug() << "textstuff" << ui.number_label->height();
	//auto fm2 = ui.number_label->fontMetrics();
	////ui.number_label->setFixedHeight(fm2.height());
	//qDebug() << "fm rect" << fm2.boundingRect("hello");
	//qDebug() << "fm capheight" << fm2.capHeight();
	//qDebug() << "height" << fm2.height() << fm2.xHeight();
}
