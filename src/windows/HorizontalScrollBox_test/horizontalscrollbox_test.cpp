#include "HorizontalScrollBox.h"
#include "narrative/NarrativeScrollItem.h"
#include <QtWidgets/QApplication> 

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HorizontalScrollBox w(nullptr);

	w.setGeometry(100, 100, 800, 250);
	for (int i = 0; i < 10; i++) {
		w.addItem(new NarrativeScrollItem("hello", "world"));
	}
	w.show();
	return a.exec();
}
