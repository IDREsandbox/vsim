#ifndef OUTLINER_EXPERIMENT_H
#define OUTLINER_EXPERIMENT_H

#include <QtWidgets/QMainWindow>
#include "ui_outliner_experiment.h"

class Outliner_experiment : public QMainWindow
{
	Q_OBJECT

public:
	Outliner_experiment(QWidget *parent = 0);
	~Outliner_experiment();

private:
	Ui::Outliner_experimentClass ui;
};

#endif // HORIZONTALSCROLLBOX_EXPERIMENT_H
