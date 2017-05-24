#include <stdexcept>
#include <iostream>
#include "narrativeoverlay_test.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	try {
		QApplication a(argc, argv);
		NarrativeOverlay_test w;
		w.show();
		return a.exec();
	}
	catch (std::exception& e) {
		std::cout << e.what() << "\n";
	}

}