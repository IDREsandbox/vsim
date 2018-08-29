#include "TimeSlider.h"
#include "ui_timeslider.h"
#include <QDebug>
#include "Core/Util.h"
#include "TimeManager.h"

TimeSlider::TimeSlider(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::TimeSlider),
	m_time(nullptr)
{
	ui->setupUi(this);
	setWindowFlags(Qt::WindowType::Window);
	setWindowTitle("Time Slider");

	ui->slider->setMinimum(-50);
	ui->slider->setMaximum(50);
	setEnabled(true);
	
	ui->slider->setValue(0); // something outrageous to cause re-scan
	ui->slider->setPageStep(10);

	connect(ui->checkBox, &QCheckBox::stateChanged, this, &TimeSlider::onCheckbox);
	connect(ui->slider, &QSlider::valueChanged, this, &TimeSlider::onValueChange);

}

TimeSlider::~TimeSlider() {
}

void TimeSlider::setTimeManager(TimeManager *time)
{
	if (m_time != nullptr) disconnect(m_time, 0, this, 0);
	m_time = time;
	if (m_time == nullptr) {
		return;
	}
	connect(m_time, &TimeManager::sYearChanged, this, &TimeSlider::onYearChange);
	connect(m_time, &TimeManager::sKeysChanged, this, &TimeSlider::onRangeChange);
	connect(m_time, &TimeManager::sTimeEnableChanged, this, &TimeSlider::onTimeEnableChange);

	setEnabled(true);

	// initialization
	onRangeChange();
	onYearChange();
	onTimeEnableChange(m_time->timeEnabled());

	ui->slider->setValue(ui->slider->minimum());
}

void TimeSlider::enableSlider(bool enable)
{
	ui->slider->setEnabled(enable);
	ui->beginLabel->setEnabled(enable);
	ui->currentLabel->setEnabled(enable);
	ui->endLabel->setEnabled(enable);
	ui->checkBox->setChecked(enable);
}

void TimeSlider::onValueChange(int year)
{
	// tell the group to change data
	if (year == 0) year = 1;
	m_time->setYear(year);
}

void TimeSlider::onCheckbox(int state)
{
	if (state == Qt::Unchecked) {
		m_time->enableTime(false);
	}
	else {
		m_time->enableTime(true);
		m_time->setYear(ui->slider->value());
	}
}

void TimeSlider::onYearChange()
{
	int year = m_time->year();
	ui->slider->setValue(year);
	ui->currentLabel->setText(QString::number(year));
}

void TimeSlider::onRangeChange()
{
	int old_value = ui->slider->value();
	std::set<int> years = m_time->keyYears();
	int min, max;
	if (years.size() == 0) {
		years = { -2000, 2000 };
		max = 2000;
		min = -2000;
	}
	else {
		min = *years.begin();
		max = *years.rbegin();
	}
	ui->slider->setMinimum(min);
	ui->slider->setMaximum(max);
	ui->slider->setTicks(years);

	if (old_value == 0 || old_value < min || old_value > max) {
		// if out of range then jump to beginning
		ui->slider->setValue(min);
	}

	ui->beginLabel->setText(QString::number(min));
	ui->endLabel->setText(QString::number(max));
}

void TimeSlider::onTimeEnableChange(bool enable)
{
	enableSlider(enable);
}
