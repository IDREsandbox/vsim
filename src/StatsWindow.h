#ifndef STATSWINDOW_H
#define STATSWINDOW_H

#include "ui_StatsWindow.h"

class StatsWindow : public QWidget {
	Q_OBJECT
public:
	StatsWindow(QWidget *parent = nullptr);

public:
	Ui::StatsWindow ui;
};

#endif