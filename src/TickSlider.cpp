#include "TickSlider.h"
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionSlider>

TickSlider::TickSlider(QWidget *parent)
	: QSlider(parent),
	m_tick_color(Qt::black)
{
	// The problem is that if we want the cool looking handle we have to enable QSlider ticks (which we don't want)
	// I was looking at ways to inject slider style options into the paintEvent and couldn't figure it out
	// Solution: just paint one tick, and paint out black mark on top of it
	setTickInterval(1000000);
	setTickPosition(QSlider::TicksAbove);

	m_hotspots = { };

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
	connect(this, &QAbstractSlider::actionTriggered, this,
		[this](int action) {

		//qDebug() << "action triggered" << (QAbstractSlider::SliderAction) action << this->value();
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

void TickSlider::setTicks(std::set<int> positions)
{
	m_hotspots.clear();
	for (auto x : positions) {
		m_hotspots.push_back(x);
	}
}


//void TickSlider::initStyleOption(QStyleOptionSlider *option) const
//{
//	QSlider::initStyleOption(option);
//	if (!option) return;
//	
//	//option->subControls |= QStyle::SC_SliderTickmarks;
//	option->tickPosition = QSlider::TicksAbove;
//}
