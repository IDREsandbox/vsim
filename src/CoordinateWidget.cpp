#include "CoordinateWidget.h"
#include <QDebug>

CoordinateWidget::CoordinateWidget(QWidget * parent)
	: QLabel(parent)
{
}

void CoordinateWidget::setCoordinate(const QVector3D &vec, const QVector3D &hpr) {
	// heading is the opposite of our yaw
	float heading = 360 - hpr.x();
	setText(QString().sprintf("x: %.1f, y: %.1f, z: %.1f | h: %.0f, p: %.0f, r: %.0f",
		vec.x(), vec.y(), vec.z(),
		heading, hpr.y(), hpr.z()
	));
}
