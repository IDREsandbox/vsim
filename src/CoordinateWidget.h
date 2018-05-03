#ifndef COORDINATEWIDGET_H
#define COORDINATEWIDGET_H

#include <QLabel>
#include <QVector3D>

class CoordinateWidget : public QLabel {
public:
	CoordinateWidget(QWidget *parent = nullptr);
	void setCoordinate(const QVector3D &vec);
};

#endif
