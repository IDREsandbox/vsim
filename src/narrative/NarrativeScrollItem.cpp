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
	qDebug() << "FOOOO";
	if (event->type() == QEvent::MouseButtonDblClick) {
		qDebug() << "GQQAQQAARRRRR";
		emit sDoubleClick();
	}
	ScrollBoxItem::mousePressEvent(event);
}

