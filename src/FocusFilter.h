#ifndef FOCUSFILTER_H
#define FOCUSFILTER_H

#include <QObject>
class FocusFilter : public QObject {
	Q_OBJECT;
public:
	FocusFilter(QObject *parent = nullptr);

	bool eventFilter(QObject * o, QEvent * e) override;
};

#endif