#include "BasicRichTextEditWidget.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QContextMenuEvent>
#include <QMenu>

BasicRichTextEditWidget::BasicRichTextEditWidget(QWidget * parent)
	: QTextEdit(parent)
{
}

void BasicRichTextEditWidget::contextMenuEvent(QContextMenuEvent * e)
{
	QMenu *menu = createStandardContextMenu(e->globalPos());
	QAction *action = menu->addAction("Paste as plain text");
	connect(action, &QAction::triggered, this, &BasicRichTextEditWidget::pastePlainText);
	menu->exec(e->globalPos());
	menu->deleteLater();
}

void BasicRichTextEditWidget::pastePlainText()
{
	auto cursor = textCursor();
	const QClipboard *clipboard = QApplication::clipboard();
	const QMimeData *mimeData = clipboard->mimeData();
	if (mimeData->hasText()) {
		cursor.insertText(mimeData->text());
	}
}
