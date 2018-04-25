#include "NumberSlider.h"

#include <QtGlobal>

NumberSlider::NumberSlider(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	m_min = 1;
	m_max = 2;
	setMin(0);
	setMax(100);

	connect(ui.spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &NumberSlider::setValue);
	connect(ui.slider, &QAbstractSlider::valueChanged, this, &NumberSlider::setValue);
}

int NumberSlider::min() const
{
	return m_min;
}

void NumberSlider::setMin(int min)
{
	m_min = min;
	ui.min->setText(QString::number(min));
	if (min > m_max) setMax(min);
	if (m_value < min) setValue(min);
	ui.slider->setMinimum(min);
	ui.spinBox->setMinimum(min);
}

int NumberSlider::max() const
{
	return m_max;
}

void NumberSlider::setMax(int max)
{
	m_max = max;
	ui.max->setText(QString::number(max));
	if (max < m_min) setMin(max);
	if (m_value > max) setValue(max);
	ui.slider->setMaximum(max);
	ui.spinBox->setMaximum(max);
}

int NumberSlider::value() const
{
	return m_value;
}

void NumberSlider::setValue(int value)
{
	int old_value = m_value;
	if (value < m_min) value = m_min;
	else if (value > m_max) value = m_max;
	ui.slider->setValue(value);
	ui.spinBox->setValue(value);
	m_value = value;
	if (old_value != value) emit sValueChanged(value);
}