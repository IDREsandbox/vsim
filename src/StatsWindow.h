#ifndef STATSWINDOW_H
#define STATSWINDOW_H

#include "ui_StatsWindow.h"

class StatsWindow : public QWidget {
	Q_OBJECT
public:
	StatsWindow(QWidget *parent = nullptr);

	// general purpose
	void clear();
	void addLine(QString str);

public:
	Ui::StatsWindow ui;
};

#endif