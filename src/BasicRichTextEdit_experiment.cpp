#include <QApplication>
#include <QWidget>
#include <QResizeEvent>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>

#include "BasicRichTextEdit.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QWidget window;

	window.resize(400, 400);
	BasicRichTextEdit *text = new BasicRichTextEdit(&window);

	auto *layout = new QVBoxLayout(&window);
	layout->addWidget(text);

	auto *btn = new QPushButton(&window);
	btn->setText("steal focus");
	layout->addWidget(btn);

	window.show();
	return a.exec();
}