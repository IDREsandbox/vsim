#ifndef ERBAR_H
#define ERBAR_H
#include <QFrame>
#include "ui_ERBar.h"

class ERBar : public QFrame {
	Q_OBJECT
public:
	ERBar(QWidget * parent = nullptr);

signals:
	void sPoked();

public:
	Ui::ERBar ui;
};

#endif