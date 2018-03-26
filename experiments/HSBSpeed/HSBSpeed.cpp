#include "HorizontalScrollBox.h"
//#include "narrative/NarrativeScrollItem.h"
//#include "narrative/NarrativeScrollBox.h"
//#include "narrative/SlideScrollBox.h"
#include "ScrollBoxItem.h"
#include <QApplication>
#include <QVBoxLayout>
#include <iostream>
#include <QPushButton>
#include <QElapsedTimer>
#include <QLabel>
#include <QDebug>

#include <QGraphicsView>
#include <QGraphicsScene>

std::vector<ScrollBoxItem*> g_items;
std::vector<QLabel*> g_labels;
int g_counter;
bool g_sel;
bool g_hide;
HorizontalScrollBox *g_box;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QWidget window(nullptr);

	window.setGeometry(100, 100, 800, 600);

	HorizontalScrollBox *normal_box = new HorizontalScrollBox(&window);
	normal_box->setGeometry(50, 200, 500, 150);
	normal_box->show();
	g_box = normal_box;

	std::vector<std::pair<size_t, ScrollBoxItem*>> insertions;
	for (int i = 0; i < 1000; i++) {
		auto item = new ScrollBoxItem();
		insertions.push_back({i, item});
		g_items.push_back(item);
		QLabel *label = new QLabel(item);
		label->setGeometry(10, 10, 100, 50);
		label->show();
		label->setText("speedy hsb test");
		g_labels.push_back(label);
	}
	normal_box->insertItems(insertions);

	QPushButton *poke_button = new QPushButton(&window);
	poke_button->setGeometry(50, 50, 100, 50);
	poke_button->setText("poke");
	poke_button->show();
	QObject::connect(poke_button, &QAbstractButton::pressed,
		[]() {
		QElapsedTimer t;
		t.start();
		g_sel = !g_sel;
		for (auto *item : g_items) {
			item->select(g_sel);
		}
		qDebug() << "style" << t.elapsed();
	});

	QPushButton *hide_button = new QPushButton(&window);
	hide_button->setGeometry(150, 50, 100, 50);
	hide_button->setText("hide");
	hide_button->show();
	QObject::connect(hide_button, &QAbstractButton::pressed,
		[]() {
		QElapsedTimer t;
		t.start();
		g_hide = !g_hide;
		for (auto *item : g_items) {
			item->setVisible(g_hide);
		}
		qDebug() << "hide show" << t.elapsed();
	});

	QPushButton *count_button = new QPushButton(&window);
	count_button->setGeometry(250, 50, 100, 50);
	count_button->setText("count");
	count_button->show();
	QObject::connect(count_button, &QAbstractButton::pressed,
		[]() {
		QElapsedTimer t;
		t.start();
		for (auto *label : g_labels) {
			double val = ((double)g_counter * 3);
			label->setText(QString::number(val, 'f', 2));
			g_counter++;
		}
		qDebug() << "count" << t.elapsed();
	});
	QPushButton *refresh_button = new QPushButton(&window);
	refresh_button->setGeometry(350, 50, 100, 50);
	refresh_button->setText("refresh");
	refresh_button->show();
	QObject::connect(refresh_button, &QAbstractButton::pressed,
		[]() {
		QElapsedTimer t;
		t.start();
		g_box->refresh();
		qDebug() << "refresh" << t.elapsed();
	});




	// graphics view?
	QGraphicsScene *scene = new QGraphicsScene(&window);
	QGraphicsView *view = new QGraphicsView();
	view->setScene(scene);

	QPushButton *count_button2 = new QPushButton(&window);
	refresh_button->setGeometry(350, 50, 100, 50);
	refresh_button->setText("refresh");
	refresh_button->show();
	QObject::connect(refresh_button, &QAbstractButton::pressed,
		[]() {
		QElapsedTimer t;
		t.start();
		g_box->refresh();
		qDebug() << "refresh" << t.elapsed();
	});


	window.show();

	return a.exec();
}
