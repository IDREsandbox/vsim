#ifndef TIMESLIDER_H
#define TIMESLIDER_H
#include <QWidget>
#include "ui_timeslider.h"

#include "ModelGroup.h"
#include <osg/Group>

// Passive view
class TimeSlider : public QWidget {
	Q_OBJECT

public:
	;
	TimeSlider(QWidget *parent = Q_NULLPTR);
	~TimeSlider();

	void setGroup(ModelGroup *group);

	void setYear(int year);
	void enable(bool);

private:
	Ui::TimeSlider ui;

	bool m_enabled;

	
	osg::ref_ptr<ModelGroup> m_group;
};

#endif // TIMESLIDER_HPP