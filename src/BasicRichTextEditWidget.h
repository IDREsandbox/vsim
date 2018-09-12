#ifndef BASICRICHTEXTEDITWIDGET_H
#define BASICRICHTEXTEDITWIDGET_H

#include <QTextEdit>

class BasicRichTextEditWidget : public QTextEdit {
public:
	BasicRichTextEditWidget(QWidget *parent = nullptr);
protected:
	void contextMenuEvent(QContextMenuEvent *e) override;
private:
	void pastePlainText();
};

#endif