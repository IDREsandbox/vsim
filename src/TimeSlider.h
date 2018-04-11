#ifndef TIMESLIDER_H
#define TIMESLIDER_H
#include <QWidget>
#include <osg/Group>
#include <memory>

class ModelGroup;

namespace Ui { class TimeSlider; }

// Time controller
class TimeSlider : public QWidget {
	Q_OBJECT

public:
	TimeSlider(QWidget *parent = Q_NULLPTR);
	~TimeSlider();

	void setGroup(ModelGroup *group);

	void enableSlider(bool);

	// make calls out to ModelGroup
	void onValueChange(int value);
	void onCheckbox(int state);

	// slots
	void onYearChange();
	void onRangeChange();
	void onTimeEnableChange(bool enable);

private:
	std::unique_ptr<Ui::TimeSlider> ui;

	bool m_enabled;

	ModelGroup *m_group;
};

#endif // TIMESLIDER_HPP