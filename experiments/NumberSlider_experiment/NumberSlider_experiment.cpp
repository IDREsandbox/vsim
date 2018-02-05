#include <QtWidgets/QApplication>
#include "NumberSlider.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	NumberSlider w;
	w.show();
	return a.exec();
}
