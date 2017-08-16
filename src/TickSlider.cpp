#include "TickSlider.h"
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionSlider>

TickSlider::TickSlider(QWidget *parent)
	: QSlider(parent),
	m_tick_color(Qt::black)
{
	setTickInterval(1000000); // biggest hack, chances are... the beginning will marked with a tick, and will cover this
	setTickPosition(QSlider::TicksAbove);

	m_hotspots = { -50, -20, -11, 11, 29, 31, 43, 50 };

	//setStyleSheet(
	//"QSlider::groove:horizontal			   "
	//"{										   "
	//"border:none;							   "
	//"	margin - top: 10px;					   "
	//"	margin - bottom: 10px;				   "
	//"	height: 10px;							   "
	//"}										   "
	//"QSlider::sub-page					   "
	//"{										   "
	//"	background: rgb(164, 192, 2);			   "
	//"}										   "
	//"QSlider::add-page					   "
	//"{										   "
	//"	background: rgb(70, 70, 70);			   "
	//"}										   "
	//"QSlider::handle						   "
	//"{										   "
	//"	background: white;						   "
	//"	border: 3px solid black;				   "
	//"	width: 60px;"
	//"	margin: -30px 0;						   "
	//"}										   "
	//);
	connect(this, &QAbstractSlider::valueChanged, this, [this](int value) {qDebug() << "value change" << value; 
		//pixelPosToRangeValue();
		qDebug() << "p from v" << valueToTickPos(value);


	});
	connect(this, &QAbstractSlider::sliderPressed, this, [this]() {qDebug() << "slider press"; });
	connect(this, &QAbstractSlider::sliderReleased, this, [this]() {qDebug() << "slider release"; });
	connect(this, &QAbstractSlider::actionTriggered, this,
		[this](int action) {
		
		qDebug() << "action triggered" << (QAbstractSlider::SliderAction) action << this->value();
		// change step size to like 30
		if (action == QAbstractSlider::SliderPageStepAdd) {
			int old_value = this->value();
			// snap to the nearest special something
			for (auto i : m_hotspots) {
				if (i > old_value) {
					// snap to this guy
					this->setValue(i);
					break;
				}
			}
			qDebug() << "old value" << old_value << "new value" << this->value();
		}
		if (action == QAbstractSlider::SliderPageStepSub) {

			int old_value = this->value();
			// snap to the nearest special something
			for (auto it = m_hotspots.rbegin(); it != m_hotspots.rend(); ++it) {
				if (*it < old_value) {
					// snap to this guy
					this->setValue(*it);
					break;
				}
			}
			qDebug() << "old value" << old_value << "new value" << this->value();
		}

	});

}

TickSlider::~TickSlider() {
	
}

void TickSlider::paintEvent(QPaintEvent * e)
{
	QSlider::paintEvent(e);

	// paint ticks
	QPainter painter(this);
	for (auto i : m_hotspots) {
		int x = valueToTickPos(i);
		QPen pen;
		pen.setWidth(1);
		pen.setColor(m_tick_color);
		painter.drawLine(x, 0, x, height() / 3);
	}
}

int TickSlider::valueToTickPos(int value) const
{
	QStyleOptionSlider opt;
	initStyleOption(&opt);
	QRect gr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
	QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
	int sliderMin, sliderMax, handleWidth;

	handleWidth = sr.width();
	sliderMin = gr.x();
	sliderMax = gr.right() - handleWidth + 1;

	int pos = QStyle::sliderPositionFromValue(minimum(), maximum(), value,
		sliderMax - sliderMin, opt.upsideDown);
	//qDebug() << "slidermin" << sliderMin << "slidermax" << sliderMax;
	return pos + sliderMin + handleWidth/2;
}

int TickSlider::pixelPosToRangeValue(int pos) const
{
	QStyleOptionSlider opt;
	initStyleOption(&opt);
	QRect gr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
	QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
	int sliderMin, sliderMax, sliderLength;

	sliderLength = sr.width();
	sliderMin = gr.x();
	sliderMax = gr.right() - sliderLength + 1;

	
	int val = QStyle::sliderValueFromPosition(minimum(), maximum(), pos - sliderMin,
		sliderMax - sliderMin, opt.upsideDown);
	qDebug() << "slider min" << sliderMin << "slider max" << sliderMax << "val" << val;
	return val;
}

void TickSlider::setTickColor(const QColor color)
{
	m_tick_color = color;
}


//void TickSlider::initStyleOption(QStyleOptionSlider *option) const
//{
//	QSlider::initStyleOption(option);
//	if (!option) return;
//	
//	//option->subControls |= QStyle::SC_SliderTickmarks;
//	option->tickPosition = QSlider::TicksAbove;
//}
