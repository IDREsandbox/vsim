#include "timeslider_experiment.h"
#include "TimeSlider.h"
//
//class TimerSlider : public QObject {
//	Q_OBJECT
//public:
//	void setYear(int);
//	int getYear() const;
//
//signals:
//	void sYearChanged(int);
//
//private:
//	int m_year;
//};
//
TimeSlider_experiment::TimeSlider_experiment(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	TimeSlider *s = new TimeSlider(centralWidget());

}

TimeSlider_experiment::~TimeSlider_experiment()
{

}
