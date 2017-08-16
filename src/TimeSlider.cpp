#include "TimeSlider.h"
#include <QDebug>

TimeSlider::TimeSlider(QWidget *parent) : QWidget(parent) {
	ui.setupUi(this);
	ui.slider->setMinimum(-50);
	ui.slider->setMaximum(50);
	
	ui.slider->setValue(1);

	ui.slider->setPageStep(10);

	connect(ui.slider, &QSlider::valueChanged, [this](int value) {
		ui.beginLabel->setText(QString::number(ui.slider->minimum()));
		ui.endLabel->setText(QString::number(ui.slider->maximum()));
		ui.currentLabel->setText(QString::number(ui.slider->value()));
	});


}

TimeSlider::~TimeSlider() {
	
}
