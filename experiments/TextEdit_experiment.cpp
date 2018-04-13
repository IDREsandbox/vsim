#include "mrichtextedit.h"
#include <QApplication>
#include <QTextDocument>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	MRichTextEdit *text = new MRichTextEdit;
	text->show();

	return a.exec();
}
