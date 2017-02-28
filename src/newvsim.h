#ifndef NEWVSIM_H
#define NEWVSIM_H

#include <QtWidgets/QMainWindow>
#include "ui_newvsim.h"

class newVSim : public QMainWindow
{
	Q_OBJECT

public:
	newVSim(QWidget *parent = 0);
	~newVSim();

private:
	Ui::newVSimClass ui;
};

#endif // NEWVSIM_H
