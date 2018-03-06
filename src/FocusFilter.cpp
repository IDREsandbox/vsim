#include "FocusFilter.h"

#include <QEvent>
#include <QFocusEvent>

FocusFilter::FocusFilter(QObject *parent)
{

}

bool FocusFilter::eventFilter(QObject * o, QEvent * e)
{
	if (e->type() == QEvent::FocusIn) {
		emit sFocusIn(o, e);
	}
	return false;
}