#ifndef BASICRICHTEXTEDIT_H
#define BASICRICHTEXTEDIT_H

#include <QTextCharFormat>

#include "ui_BasicRichTextEdit.h"

class ABCDTextEdit : public QTextEdit {
public:
	ABCDTextEdit(QWidget *parent = nullptr) {}
protected:
	//void contextMenuEvent(QContextMenuEvent *e) override;
private:
	//void pastePlainText();
};

class BasicRichTextEdit : public QWidget {
	Q_OBJECT;
public:
	BasicRichTextEdit(QWidget *parent = 0);

	//QString toPlainText() const;
	//QString toHtml() const;
	//QTextDocument *document();
	//QTextCursor    textCursor();
	//void setTextCursor(const QTextCursor& cursor);
	//void setStyleSheet_textEdit(const std::string str);

    void setText(const QString &text);
	void setHtml(const QString &html);
	QString html() const;

    //void textRemoveFormat();
    //void textRemoveAllFormat();
    void textBold();
    void textUnderline();
    //void textStrikeout();
    void textItalic();

	void setReadOnly(bool read_only);

private:
	void onAnyChange();

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

private:
	class FocusFilter : public QObject {
	public:
		FocusFilter(BasicRichTextEdit *boss);
		bool eventFilter(QObject *obj, QEvent *e) override;
	private:
		BasicRichTextEdit *m_boss;
	};

private:
	Ui::BasicRichTextEdit ui;
	bool m_read_only;
};

#endif
