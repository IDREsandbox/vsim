#ifndef HORIZONTALSCROLLBOX_EXPERIMENT_H
#define HORIZONTALSCROLLBOX_EXPERIMENT_H

#include <QtWidgets/QMainWindow>
#include "ui_HorizontalScrollBox_experiment.h"

class HorizontalScrollBox_experiment : public QMainWindow
{
	Q_OBJECT

public:
	HorizontalScrollBox_experiment(QWidget *parent = 0);
	~HorizontalScrollBox_experiment();

private:
	Ui::HorizontalScrollBox_experimentClass ui;
};

#endif // HORIZONTALSCROLLBOX_EXPERIMENT_H
