#include "TimeSlider.h"
#include "ui_timeslider.h"
#include <QDebug>
#include "Util.h"

TimeSlider::TimeSlider(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::TimeSlider),
	m_group(nullptr)
{
	ui->setupUi(this);
	ui->slider->setMinimum(-50);
	ui->slider->setMaximum(50);
	setEnabled(true);
	
	ui->slider->setValue(1);
	ui->slider->setPageStep(10);

	this->setWindowFlags(Qt::Dialog);

	connect(ui->checkBox, &QCheckBox::stateChanged, this, &TimeSlider::onCheckbox);
	connect(ui->slider, &QSlider::valueChanged, this, &TimeSlider::onValueChange);

	enableSlider(false);
}

TimeSlider::~TimeSlider() {
	
}

void TimeSlider::setGroup(ModelGroup * group)
{
	if (m_group.get() != nullptr) disconnect(m_group.get(), 0, this, 0);
	m_group = group;
	if (group == nullptr) {
		return;
	}
	connect(m_group, &ModelGroup::sYearChange, this, &TimeSlider::onYearChange);
	connect(m_group, &ModelGroup::sUserValueChanged, this,
		[this](osg::Node *node, std::string name) {
			if (name == "yearBegin" || name == "yearEnd") {
				onRangeChange();
			}
		});
	connect(m_group, &ModelGroup::sTimeEnableChange, this, &TimeSlider::onTimeEnableChange);

	// hacky initialization
	onRangeChange();
	onYearChange();
}

void TimeSlider::enableSlider(bool enable)
{
	ui->slider->setEnabled(enable);
	ui->beginLabel->setEnabled(enable);
	ui->currentLabel->setEnabled(enable);
	ui->endLabel->setEnabled(enable);
	ui->checkBox->setChecked(enable);
}

void TimeSlider::onValueChange(int value)
{
	// tell the group to change data
	if (value == 0) value = 1;
	m_group->setYear(value);
}

void TimeSlider::onCheckbox(int state)
{
	if (state == Qt::Unchecked) {
		m_group->enableTime(false);
	}
	else {
		m_group->enableTime(true);
		m_group->setYear(ui->slider->value());
	}
}

void TimeSlider::onYearChange()
{
	int year = m_group->getYear();
	ui->slider->setValue(year);
	ui->currentLabel->setText(QString::number(year));
}

void TimeSlider::onRangeChange()
{
	std::set<int> years = m_group->getKeyYears();
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

	ui->beginLabel->setText(QString::number(min));
	ui->endLabel->setText(QString::number(max));
}

void TimeSlider::onTimeEnableChange(bool enable)
{
	enableSlider(enable);
}
