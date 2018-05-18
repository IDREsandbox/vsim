#include "Canvas/CanvasContainer.h"
#include "Canvas/CanvasScene.h"
//#include "narrative/NarrativeScrollItem.h"
//#include "narrative/NarrativeScrollBox.h"
//#include "narrative/SlideScrollBox.h"
#include "ScrollBoxItem.h"
#include <QApplication>
#include <iostream>
#include <QTextDocument>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QWidget window(nullptr);

	window.setGeometry(100, 100, 800, 600);
	window.setStyleSheet("background:rgba(100, 100, 100);");

	QVBoxLayout *verticalLayout = new QVBoxLayout(&window);
	window.setLayout(verticalLayout);

	CanvasScene *canvas = new CanvasScene(&window);
	canvas->setBaseHeight(600);
	canvas->setEditable(true);

	auto text = std::make_shared<CanvasLabel>();
	text->setVAlign(Qt::AlignCenter);
	QObject::connect(text->document(), &QTextDocument::undoCommandAdded,
		[text]() {
		qDebug() << "undo command added";
		//qDebug() << "c has focus" << text->hasFocus();
	});
	text->document()->setPlainText("helloooo\nWORLd");

	//QObject::connect(canvas, &CanvasScene::changed, [text]() {
	//	qDebug() << "anyc" << text->hasFocus();
	//});
	text->setRect(QRectF(0, 0, .2, .2));
	text->setBrush(QBrush(QColor(255, 0, 0, 100)));
	text->setEditable(true);
	//rect->debugPaint(true);
	canvas->addItem(text);

	auto rect2 = std::make_shared<CanvasItem>();
	rect2->setRect(QRectF(-.2, -.2, .2, .2));
	rect2->setBrush(QBrush(QColor(0, 255, 0, 100)));
	rect2->setEditable(true);
	rect2->setPrefersFixedRatio(true);
	rect2->debugPaint(true);
	rect2->setPen(QPen(QBrush(QColor(0, 0, 0)), canvas->toScene(10)));
	canvas->addItem(rect2);

	auto rect3 = std::make_shared<CanvasItem>();
	rect3->setRect(QRectF(-.2, 0, .2, .2));
	rect3->setBrush(QBrush(QColor(0, 0, 255, 255)));
	rect3->setPen(QPen(QBrush(QColor(0, 0, 0)), canvas->toScene(10)));
	rect3->setBorderAround(true);
	rect3->setEditable(true);
	rect3->setPrefersFixedRatio(false);
	canvas->addItem(rect3);

	auto image = std::make_shared<CanvasImage>();
	image->move(.2, -.2);
	image->setPixmap(QPixmap("assets/karnak.jpg"));
	image->setPen(QPen(QBrush(QColor(0, 0, 0)), canvas->toScene(10)));
	image->setEditable(true);
	canvas->addItem(image);

	CanvasContainer *container = new CanvasContainer(&window);
	container->setStyleSheet("background:rgba(0, 0, 0, 0);");
	verticalLayout->addWidget(container);
	container->setScene(canvas);
	container->setEditable(true);
	container->show();

	window.show();

	qDebug() << "current dir?" << QDir::currentPath();
	return a.exec();
}
