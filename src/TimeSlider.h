#ifndef TIMESLIDER_H
#define TIMESLIDER_H
#include <QWidget>
#include "ui_timeslider.h"

class TimeSlider : public QWidget {
	Q_OBJECT

public:
	TimeSlider(QWidget *parent = Q_NULLPTR);
	~TimeSlider();

	void setTimeHandler();

private:
	Ui::TimeSlider ui;
};

#endif // TIMESLIDER_HPP