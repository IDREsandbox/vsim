#include "StatsWindow.h"

StatsWindow::StatsWindow(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::Window);
	ui.setupUi(this);
}

void StatsWindow::clear()
{
	ui.other->clear();
}

void StatsWindow::addLine(QString str)
{
	ui.other->append(str + "\n");
}
