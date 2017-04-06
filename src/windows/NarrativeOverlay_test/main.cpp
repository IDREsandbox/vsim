#include "narrativeoverlay_test.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	NarrativeOverlay_test w;
	w.show();
	return a.exec();
}
