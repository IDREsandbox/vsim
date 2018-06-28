#include "ERBar.h"

#include <QDebug>
#include "FocusFilter.h"

ERBar::ERBar(QWidget * parent)
	: QFrame(parent)
{
	ui.setupUi(this);

	FocusFilter *ff = new FocusFilter(this);
	this->installEventFilter(ff);
	ui.local->installEventFilter(ff);
	ui.global->installEventFilter(ff);
	for (QObject *obj : children()) {
		obj->installEventFilter(ff);
	}
	connect(ff, &FocusFilter::sFocusIn, this, &ERBar::sPoked);
}
