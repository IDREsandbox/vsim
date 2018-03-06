#include "narrative/CanvasContainer.h"
//#include "narrative/NarrativeScrollItem.h"
//#include "narrative/NarrativeScrollBox.h"
//#include "narrative/SlideScrollBox.h"
#include "ScrollBoxItem.h"
#include <QApplication>
#include <iostream>
#include <QTextDocument>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QWidget window(nullptr);

	window.setGeometry(100, 100, 800, 600);
	window.setStyleSheet("background:rgba(100, 100, 100);");

	QVBoxLayout *verticalLayout = new QVBoxLayout(&window);
	window.setLayout(verticalLayout);

	CanvasContainer *canvas = new CanvasContainer(&window);
	canvas->setStyleSheet("background:rgba(0, 0, 0, 0);");
	verticalLayout->addWidget(canvas);
	canvas->setBaseHeight(600);
	canvas->setEditable(true);

	RectItem *rect;
	TextRect *text;

	text = new TextRect();
	text->setBaseHeight(600);
	QObject::connect(text->m_text->document(), &QTextDocument::undoCommandAdded,
		[text]() {
		qDebug() << "undo command added";
	});

	rect = text;
	rect->setRect(QRectF(0, 0, .2, .2));
	rect->setBrush(QBrush(QColor(255,0,0)));
	rect->setEditable(true);
	canvas->scene()->addItem(rect);

	RectItem *rect2 = new RectItem();
	rect2->setRect(QRectF(-.2, -.2, .2, .2));
	rect2->setBrush(QBrush(QColor(0, 255, 0)));
	rect2->setEditable(true);
	canvas->scene()->addItem(rect2);

	window.show();

	return a.exec();
}
