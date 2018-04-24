#include "StatsWindow.h"

StatsWindow::StatsWindow(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::Window);
	ui.setupUi(this);
}