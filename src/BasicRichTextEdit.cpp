#include "BasicRichTextEdit.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QDebug>
#include <QBuffer>
#include <QTextList>
#include <QMenu>

BasicRichTextEdit::BasicRichTextEdit(QWidget *parent)
	: QWidget(parent),
	m_read_only(false)
{
	ui.setupUi(this);

	ui.text->setTabStopWidth(40);

	auto *ff = new FocusFilter(this);
	ui.text->installEventFilter(ff);

	connect(ui.text, &QTextEdit::currentCharFormatChanged,
		this, &BasicRichTextEdit::onAnyChange);

	// bold, italic & underline
	ui.bold->setShortcut(Qt::CTRL + Qt::Key_B);
	ui.italic->setShortcut(Qt::CTRL + Qt::Key_I);
	ui.underline->setShortcut(Qt::CTRL + Qt::Key_U);

	ui.bold->setFocusPolicy(Qt::NoFocus);
	ui.italic->setFocusPolicy(Qt::NoFocus);
	ui.underline->setFocusPolicy(Qt::NoFocus);

	connect(ui.bold, &QAbstractButton::clicked, this, &BasicRichTextEdit::textBold);
	connect(ui.italic, &QAbstractButton::clicked, this, &BasicRichTextEdit::textItalic);
	connect(ui.underline, &QAbstractButton::clicked, this, &BasicRichTextEdit::textUnderline);
	//connect(f_strikeout, SIGNAL(clicked()), this, SLOT(textStrikeout()));

	//QAction *removeFormat = new QAction(tr("Remove character formatting"), this);
	//removeFormat->setShortcut(QKeySequence("CTRL+M"));
	//connect(removeFormat, SIGNAL(triggered()), this, SLOT(textRemoveFormat()));
	//ui.text->addAction(removeFormat);

	//QAction *removeAllFormat = new QAction(tr("Remove all formatting"), this);
	//connect(removeAllFormat, SIGNAL(triggered()), this, SLOT(textRemoveAllFormat()));
	//ui.text->addAction(removeAllFormat);

	onAnyChange();
}


void BasicRichTextEdit::setText(const QString & text)
{
	ui.text->setText(text);
}

void BasicRichTextEdit::setHtml(const QString & html)
{
	ui.text->setHtml(html);
}

QString BasicRichTextEdit::html() const
{
	return ui.text->toHtml();
}

void BasicRichTextEdit::textBold() {
	QTextCharFormat fmt;
	fmt.setFontWeight(ui.bold->isChecked() ? QFont::Bold : QFont::Normal);
	mergeFormatOnWordOrSelection(fmt);
}

void BasicRichTextEdit::textUnderline() {
	QTextCharFormat fmt;
	fmt.setFontUnderline(ui.underline->isChecked());
	mergeFormatOnWordOrSelection(fmt);
}

void BasicRichTextEdit::textItalic() {
	QTextCharFormat fmt;
	fmt.setFontItalic(ui.italic->isChecked());
	mergeFormatOnWordOrSelection(fmt);
}

void BasicRichTextEdit::setReadOnly(bool read_only)
{
	m_read_only = read_only;
	onAnyChange();
}

void BasicRichTextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format) {
	QTextCursor cursor = ui.text->textCursor();
	cursor.mergeCharFormat(format);
	ui.text->setTextCursor(cursor);
	ui.text->setFocus(Qt::TabFocusReason);

	// don't do the word thing, it's weird
	//cursor.beginEditBlock();
	//if (!cursor.hasSelection()) {
	//	cursor.select(QTextCursor::WordUnderCursor);
	//}
	//cursor.mergeCharFormat(format);
	//ui.text->mergeCurrentCharFormat(format);
	//cursor.endEditBlock();
	//ui.text->setFocus(Qt::TabFocusReason);
}

void BasicRichTextEdit::onAnyChange()
{
	// toolbar buttons
	auto fmt = ui.text->currentCharFormat();
	ui.bold->setChecked(fmt.fontWeight() > 50);
	ui.italic->setChecked(fmt.fontItalic());
	ui.underline->setChecked(fmt.fontUnderline());

	// toolbar show conditions
	// 1. focused
	// 2. not read only
	bool show_bar = ui.text->hasFocus() && !m_read_only;
	ui.toolbar->setVisible(show_bar);

	// read only
	ui.text->setReadOnly(m_read_only);
}

BasicRichTextEdit::FocusFilter::FocusFilter(BasicRichTextEdit *boss)
	: QObject(boss),
	m_boss(boss)
{
}

bool BasicRichTextEdit::FocusFilter::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::Type::FocusIn) {
		m_boss->onAnyChange();
	}
	else if (e->type() == QEvent::Type::FocusOut) {
		m_boss->onAnyChange();
	}
	return false;
}