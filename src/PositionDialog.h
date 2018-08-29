#ifndef POSITIONDIALOG_H
#define POSITIONDIALOG_H

#include "ui_PositionDialog.h"

#include <QVector3D>

// 0 dependencies very simple dialog
class PositionDialog : public QDialog {
	Q_OBJECT;
public:
	PositionDialog(QWidget *parent = nullptr);

	void setPosition(QVector3D position);
	QVector3D position() const;

	// in degrees
	void setOrientationDeg(QVector3D position);
	QVector3D orientationDeg() const;

public:
	Ui::PositionDialog ui;
};

#endif