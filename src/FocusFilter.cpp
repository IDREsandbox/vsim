#include "FocusFilter.h"

#include <QEvent>

FocusFilter::FocusFilter(QObject *parent)
{

}

bool FocusFilter::eventFilter(QObject * o, QEvent * e)
{
	if (e->type()) {

	}
	return false;
}