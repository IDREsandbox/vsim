#include "PositionDialog.h"

PositionDialog::PositionDialog(QWidget *parent)
{
	ui.setupUi(this);
}

void PositionDialog::setPosition(QVector3D position)
{
	ui.x_spin->setValue(position.x());
	ui.y_spin->setValue(position.y());
	ui.z_spin->setValue(position.z());
}

QVector3D PositionDialog::position() const
{
	return QVector3D(
		ui.x_spin->value(),
		ui.y_spin->value(),
		ui.z_spin->value());
}

void PositionDialog::setOrientationDeg(QVector3D position)
{
	ui.h_spin->setValue(position.x());
	ui.p_spin->setValue(position.y());
	ui.r_spin->setValue(position.z());
}

QVector3D PositionDialog::orientationDeg() const
{
	return QVector3D(
		ui.h_spin->value(),
		ui.p_spin->value(),
		ui.r_spin->value());
}
