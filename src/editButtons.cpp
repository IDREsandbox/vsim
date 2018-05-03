#include "editButtons.h"
#include <QDebug>
#include "FocusFilter.h"

editButtons::editButtons(QWidget* parent) : QFrame(parent)
{
	ui.setupUi(this);
	setStyleSheet("background: rgba(0, 0, 0, 70); color: rgb(255, 255, 255); border-color: rgb(238, 238, 238);");

	FocusFilter *ff = new FocusFilter(this);
	this->installEventFilter(ff);
	for (QObject *obj : children()) {
		obj->installEventFilter(ff);
	}
	connect(ff, &FocusFilter::sFocusIn, this, &editButtons::sPoked);
}

QPushButton *editButtons::button(LabelType type)
{
	switch (type) {
	case LabelType::BODY:
		return ui.body;
	case LabelType::HEADER1:
		return ui.head1;
	case LabelType::HEADER2:
		return ui.head2;
	case LabelType::LABEL:
		return ui.label;
	default:
		return nullptr;
	}
	return nullptr;
}
