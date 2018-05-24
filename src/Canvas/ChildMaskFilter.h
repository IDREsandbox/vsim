#ifndef CHILDMASKFILTER_H
#define CHILDMASKFILTER_H

#include <QObject>
#include <QWidget>

// Automatically sets mask to children
// It's hard to know when children change. paint event is the most for sure
// way to know but it's inefficient. For our current uses LayoutRequest and
// ResizeEvent are good enough

// Uses:
// Canvas' internal MainWindow has a toolbar. We need to mask the
// rest of the widget away.

class ChildMaskFilter : public QObject {
public:
	ChildMaskFilter(QObject *parent = nullptr);
	bool eventFilter(QObject *object, QEvent *event) override;

private:
	void calcMask(QWidget *w);
};

#endif