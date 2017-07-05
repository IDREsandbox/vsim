#include "NarrativeScrollItem.h"

NarrativeScrollItem::NarrativeScrollItem() {
	ui.setupUi(this);
}

void NarrativeScrollItem::setInfo(const std::string & title, const std::string & description)
{
	ui.title->setText(QString::fromStdString(title));
	ui.description->setText(QString::fromStdString(description));
}

void NarrativeScrollItem::mousePressEvent(QMouseEvent * event)
{
	if (event->type() == QEvent::MouseButtonDblClick) {
		emit sDoubleClick();
	}
	ScrollBoxItem::mousePressEvent(event);
}

