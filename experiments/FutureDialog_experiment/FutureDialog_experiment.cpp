#include "Multithread.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Multithread w;
	w.show();
	return a.exec();
}
