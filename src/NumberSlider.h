#ifndef NUMBERSLIDER_H
#define NUMBERSLIDER_H

#include "ui_NumberSlider.h"

class NumberSlider : public QWidget {
	Q_OBJECT;
	Q_PROPERTY(int min READ min WRITE setMin DESIGNABLE true);
	Q_PROPERTY(int max READ min WRITE setMin DESIGNABLE true);
	Q_PROPERTY(int value READ value WRITE setValue DESIGNABLE true);

public:
	NumberSlider(QWidget *parent = nullptr);

	int min() const;
	void setMin(int min);
	int max() const;
	void setMax(int max);
	int value() const;
	void setValue(int value);

signals:
	void sValueChanged(int value);

private:
	Ui::Form ui;
	int m_min;
	int m_max;
	int m_value;
	//bool m_set_slider;
	//bool m_set_spinbox;
};

#endif
