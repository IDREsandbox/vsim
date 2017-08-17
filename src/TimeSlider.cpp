#include "TimeSlider.h"
#include <QDebug>

TimeSlider::TimeSlider(QWidget *parent)
	: QWidget(parent),
	m_group(nullptr)
{
	ui.setupUi(this);
	ui.slider->setMinimum(-50);
	ui.slider->setMaximum(50);
	setEnabled(true);
	
	ui.slider->setValue(1);

	ui.slider->setPageStep(10);

	connect(ui.checkBox, &QCheckBox::stateChanged, [this]() {
		enable(ui.checkBox->isChecked());
	});

	connect(ui.slider, &QSlider::valueChanged, [this](int value) {
		ui.beginLabel->setText(QString::number(ui.slider->minimum()));
		ui.endLabel->setText(QString::number(ui.slider->maximum()));
		ui.currentLabel->setText(QString::number(ui.slider->value()));
	});
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
	connect(m_group, &ModelGroup::sYearChange, this, &TimeSlider::setYear);

	//m_group->
}

void TimeSlider::setYear(int year)
{
	if (year == 0) {
		ui.slider->setEnabled(false);
		ui.beginLabel->setEnabled(false);
		ui.endLabel->setEnabled(false);
		return;
	}
	ui.slider->setValue(year);
	ui.currentLabel->setText(QString::number(year));
}

void TimeSlider::enable(bool enable)
{
	if (enable) {
		ui.slider->setEnabled(true);
		ui.beginLabel->setEnabled(true);
		ui.currentLabel->setEnabled(true);
		ui.endLabel->setEnabled(true);
		ui.checkBox->setChecked(true);
	}
	else {
		ui.slider->setEnabled(false);
		ui.beginLabel->setEnabled(false);
		ui.currentLabel->setEnabled(false);
		ui.endLabel->setEnabled(false);
		ui.checkBox->setChecked(false);
	}
}
