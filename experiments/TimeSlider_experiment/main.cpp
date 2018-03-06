#include "timeslider_experiment.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TimeSlider_experiment w;
	w.show();
	return a.exec();
}
