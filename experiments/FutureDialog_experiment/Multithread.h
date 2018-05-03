#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Multithread.h"

class Multithread : public QMainWindow
{
	Q_OBJECT

public:
	Multithread(QWidget *parent = Q_NULLPTR);

private:
	Ui::MultithreadClass ui;
};
