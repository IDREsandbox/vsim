#include "HorizontalScrollBox.h"
//#include "narrative/NarrativeScrollItem.h"
#include "narrative/NarrativeScrollBox.h"
#include "narrative/SlideScrollBox.h"
#include <QtWidgets/QApplication> 
#include <QtWidgets/QVBoxLayout>
#include "Foo.hpp"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QWidget window(nullptr);

	window.setGeometry(100, 100, 800, 750);

	window.setStyleSheet("background-color: rgba(40,40,40,200);");
	QVBoxLayout *verticalLayout = new QVBoxLayout(&window);
	window.setLayout(verticalLayout);

	HorizontalScrollBox *normal_box = new HorizontalScrollBox(&window);
	//HorizontalScrollBox *narrative_box = new HorizontalScrollBox(&window);
	NarrativeScrollBox *narrative_box = new NarrativeScrollBox(&window);
	SlideScrollBox *slide_box = new SlideScrollBox(&window);

	// todo
	//NarrativeScrollBox *narrative_box = new NarrativeScrollBox(&window);
	//SlideScrollBox *slide_box = new SlideScrollBox(&window);

	verticalLayout->addWidget(normal_box);
	verticalLayout->addWidget(narrative_box);
	verticalLayout->addWidget(slide_box);

	
	//for (int i = 0; i < 4; i++) {
	//	normal_box->addItem(new NarrativeScrollItem("hello", "world"));
	//}
	//for (int i = 0; i < 10; i++) {
	//	narrative_box->addItem("hello", "world2");
	//}
	//for (int i = 0; i < 4; i++) {
	//	narrative_box->addItem("hello", "world");
	//}
	for (int i = 0; i < 4; i++) {
		//slide_box->addItem(new NarrativeScrollItem("hello", "world"));
		slide_box->addItem();
	}

	window.show();

	return a.exec();
}
