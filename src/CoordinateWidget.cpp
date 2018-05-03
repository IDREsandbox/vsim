#include "CoordinateWidget.h"
#include <QDebug>

CoordinateWidget::CoordinateWidget(QWidget * parent)
	: QLabel(parent)
{
}

void CoordinateWidget::setCoordinate(const QVector3D &vec) {
	setText(QString().sprintf("x: %.1f, y: %.1f, z: %.1f",
		vec.x(), vec.y(), vec.z()));
}
