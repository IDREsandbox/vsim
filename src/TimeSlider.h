#ifndef TIMESLIDER_H
#define TIMESLIDER_H
#include <QWidget>
#include <osg/Group>
#include <memory>

class TimeManager;

namespace Ui { class TimeSlider; }

// Time controller
class TimeSlider : public QWidget {
	Q_OBJECT

public:
	TimeSlider(QWidget *parent = Q_NULLPTR);
	~TimeSlider();

	void setTimeManager(TimeManager *timer);

	void enableSlider(bool);

	// ui -> time manager
	void onValueChange(int action);
	void onCheckbox(int state);

	// time manager -> ui
	void onYearChange();
	void onRangeChange();
	void onTimeEnableChange(bool enable);

private:
	std::unique_ptr<Ui::TimeSlider> ui;

	bool m_enabled;

	TimeManager *m_time;
};

#endif // TIMESLIDER_HPP