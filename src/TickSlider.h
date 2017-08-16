#ifndef TICKSLIDER_H
#define TICKSLIDER_H
#include <QSlider>
#include <QDebug>

class TickSlider : public QSlider {
	Q_OBJECT
	Q_PROPERTY(QColor tickColor MEMBER m_tick_color WRITE setTickColor)

public:
	TickSlider(QWidget *parent = Q_NULLPTR);
	~TickSlider();

	std::vector<int> m_hotspots;

	// Draw tickmarks
	void paintEvent(QPaintEvent *e) override;

	// Copy of Qt's QSliderPrivate::pixelPosToRangeValue, but going the other way
	int valueToTickPos(int value) const;
	int pixelPosToRangeValue(int pos) const;

	void setTickColor(const QColor color);

private:
	QColor m_tick_color;
};

#endif // TICKSLIDER_HPP