#include "HorizontalScrollBox.h"
//#include "narrative/NarrativeScrollItem.h"
#include "narrative/NarrativeScrollBox.h"
#include "narrative/SlideScrollBox.h"
#include <QtWidgets/QApplication> 
#include <QtWidgets/QVBoxLayout>
//#include "Foo.hpp"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QWidget window(nullptr);

	window.setGeometry(100, 100, 800, 750);

	window.setStyleSheet(".QWidget { background-color: rgba(40,40,40,255); }");
	QVBoxLayout *verticalLayout = new QVBoxLayout(&window);
	window.setLayout(verticalLayout);

	HorizontalScrollBox *normal_box = new HorizontalScrollBox(&window);
	
	NarrativeScrollBox *narrative_box = new NarrativeScrollBox(&window);
	// wire the box to itself
	QObject::connect(narrative_box, &NarrativeScrollBox::sNew, [narrative_box]() { narrative_box->addItem("new", "slide"); });
	QObject::connect(narrative_box, &NarrativeScrollBox::sDelete, [narrative_box]() { narrative_box->deleteSelection(); });

	SlideScrollBox *slide_box = new SlideScrollBox(&window);
	QObject::connect(slide_box, &SlideScrollBox::sSetDuration, [slide_box](float d) { slide_box->setDuration(d); });
	QObject::connect(slide_box, &SlideScrollBox::sSetTransitionDuration, [slide_box](float d) { slide_box->setTranstionDuration(d); });

	verticalLayout->addWidget(normal_box);
	verticalLayout->addWidget(narrative_box);
	verticalLayout->addWidget(slide_box);

	//slide_box->setStyleSheet("background-color: rgba(0,0,0,200);");
	//narrative_box->setStyleSheet("QMenu { color: rgb(40,40,40); }");
	
	for (int i = 0; i < 4; i++) {
		normal_box->addItem(new ScrollBoxItem());
	}
	for (int i = 0; i < 4; i++) {
		narrative_box->addItem("hello", "world");
	}
	for (int i = 0; i < 4; i++) {
		slide_box->addItem();
	}

	window.show();

	return a.exec();
}
