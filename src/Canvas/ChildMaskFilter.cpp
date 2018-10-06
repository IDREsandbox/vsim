#include "ChildMaskFilter.h"

#include <QResizeEvent>

ChildMaskFilter::ChildMaskFilter(QObject *parent)
	: QObject(parent)
{
}

bool ChildMaskFilter::eventFilter(QObject *object, QEvent *e)
{
	QEvent::Type type = e->type();
	QWidget *w = dynamic_cast<QWidget*>(object);
	switch (type) {
	case QEvent::LayoutRequest:
	case QEvent::Resize:
	case QEvent::Paint:
		// there is some double-painting because of this. solution?
		calcMask(w);
		break;
	default:
		break;
	}
	return false;
}

void ChildMaskFilter::calcMask(QWidget *w)
{
	if (!w) return;
	auto cr = w->childrenRegion();
	auto current = w->mask();
	if (cr == current) return;

	if (cr.isNull()) {
		w->setAttribute(Qt::WA_TransparentForMouseEvents);
	}
	else {
		w->setAttribute(Qt::WA_TransparentForMouseEvents, false);
		w->setMask(cr);
	}
}

