#include "HorizontalScrollBox.h"
//#include "narrative/NarrativeScrollItem.h"
//#include "narrative/NarrativeScrollBox.h"
//#include "narrative/SlideScrollBox.h"
#include "ScrollBoxItem.h"
#include <QApplication>
#include <QVBoxLayout>
#include <iostream>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QWidget window(nullptr);

	qDebug() << "before set geom";
	window.setGeometry(100, 100, 800, 200);
	qDebug() << "after set geom";

	window.setStyleSheet(".QWidget { background-color: rgba(40,40,40,255); }");
	QVBoxLayout *verticalLayout = new QVBoxLayout(&window);
	window.setLayout(verticalLayout);

	HorizontalScrollBox *normal_box = new HorizontalScrollBox(&window);
	verticalLayout->addWidget(normal_box);

	std::vector<std::pair<size_t, ScrollBoxItem*>> items;
	for (int i = 0; i < 30; i++) {
		auto item = new ScrollBoxItem();
		items.push_back({i, item});
	}
	normal_box->insertItems(items);

	//NarrativeScrollBox *narrative_box = new NarrativeScrollBox(&window);
	//// wire the box to itself
	//QObject::connect(narrative_box, &NarrativeScrollBox::sNew, 
	//	[narrative_box]() { 
	//	qDebug() << "new";
	//	narrative_box->addNewItem();
	//});
	//QObject::connect(narrative_box, &NarrativeScrollBox::sDelete, [narrative_box]() { narrative_box->deleteSelection(); });
	//QObject::connect(narrative_box, &NarrativeScrollBox::sMove,
	//	[](std::set<int> from, int to) {
	//	qDebug() << "MOVE EVENT" << "TO:" << to;
	//	std::cout << "FROM:";
	//	for (auto i : from) std::cout << " " << i;
	//	std::cout << "\n";
	//});
	//
	////a.setStartDragDistance(10);
	////a.setStartDragTime(100);

	//SlideScrollBox *slide_box = new SlideScrollBox(&window);
	////QObject::connect(slide_box, &SlideScrollBox::sSetDuration, [slide_box](float d) { slide_box->setDuration(d); });
	////QObject::connect(slide_box, &SlideScrollBox::sSetTransitionDuration, [slide_box](float d) { slide_box->setTranstionDuration(d); });

	//verticalLayout->addWidget(normal_box);
	//verticalLayout->addWidget(narrative_box);
	//verticalLayout->addWidget(slide_box);

	//QLabel *emptyspace = new QLabel(&window);
	//emptyspace->setGeometry(0, 300, 200, 200);
	//emptyspace->setFixedHeight(100);
	//emptyspace->setStyleSheet("background-color: rgb(255,0,0)");
	//verticalLayout->addWidget(emptyspace);

	////slide_box->setStyleSheet("background-color: rgba(0,0,0,200);");
	////narrative_box->setStyleSheet("QMenu { color: rgb(40,40,40); }");

	//for (int i = 0; i < 8; i++) {
	//	narrative_box->addNewItem();
	//}
	////for (int i = 0; i < 4; i++) {
	////	narrative_box->addItem("hello", "world");
	////}
	////for (int i = 0; i < 4; i++) {
	////	slide_box->addItem();
	////}

	QTimer timer;
	timer.setInterval(0);
	QObject::connect(&timer, &QTimer::timeout, [&]() {
		//normal_box->update();
		//qDebug() << "cat" << x; x++;
	});
	timer.start();

	QAction *a_visible = new QAction(&window);
	a_visible->setShortcut(QKeySequence(Qt::Key_O));
	window.addAction(a_visible);
	QObject::connect(a_visible, &QAction::triggered, [&]() {
		auto items = normal_box->visibleItems();
		qDebug() << "overlap" << QVector<size_t>::fromStdVector(items);
	});

	window.show();

	return a.exec();
}
