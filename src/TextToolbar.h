#ifndef TEXTTOOLBAR_H
#define TEXTTOOLBAR_H

#include <QWidget>
#include <QGraphicsTextItem>

#include "ui_TextToolbar.h"

class TextToolbar : public QWidget {
	Q_OBJECT
public:
	TextToolbar(QWidget *parent = 0);

	void setTextEdit(QGraphicsTextItem *doc);

private:
	Ui_TextToolbar ui;
};

#endif
