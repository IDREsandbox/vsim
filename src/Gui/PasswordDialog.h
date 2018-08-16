#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QPushButton>

#include "ui_PasswordDialog.h"

class CreatePasswordDialog : public QDialog {
	Q_OBJECT;
public:
	CreatePasswordDialog(QWidget *parent = nullptr);

	//void setUseCheckbox(bool use_checkbox);
	//void setUsePassword(bool use);
	bool usePassword() const;
	QString password() const;

	void setLabel(QString text);

//public:
//	static QString getPasswordCheckable(bool *ok,
//		bool *no_pw);
//	static QString getPassword(bool *ok, QString title,
//		QString label);

public:

private:
	void onUseChange();

public:
	Ui::PasswordDialog ui;
	bool m_use;
};


class TestPasswordDialog : public QDialog {
	Q_OBJECT;
public:
	TestPasswordDialog(QWidget *parent = nullptr);
	void setLabel(QString test);

	void showSkipButton(bool show);

	// testCallback(QString pw) -> bool
	//  return true if successful
	void setTestCallback(std::function<bool(QString)> f);

	void tryAccept();

public:
	Ui::PasswordDialog ui;
	std::function<bool(QString)> m_func;
	QPushButton *m_skip_button;
};

#endif