#ifndef TIMESLIDER_EXPERIMENT_H
#define TIMESLIDER_EXPERIMENT_H

#include <QtWidgets/QMainWindow>
#include "ui_timeslider_experiment.h"

class TimeSlider_experiment : public QMainWindow
{
	Q_OBJECT

public:
	TimeSlider_experiment(QWidget *parent = 0);
	~TimeSlider_experiment();

private:
	Ui::TimeSlider_experimentClass ui;
};

#endif // TIMESLIDER_EXPERIMENT_H
