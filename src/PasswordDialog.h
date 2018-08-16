#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>

#include "ui_PasswordDialog.h"

class PasswordDialog : public QDialog {
	Q_OBJECT;
public:
	PasswordDialog(QWidget *parent = nullptr);

	void setUsePassword(bool use);
	bool usePassword() const;
	QString password() const;

	void setLabelText(QString text);
	void showLabel(bool show);

public:
	static QString getPasswordCheckable(bool *ok,
		bool *no_pw);
	static QString getPassword(bool *ok, QString title,
		QString label);

private:
	void onUseChange(bool use);

public:
	Ui::PasswordDialog ui;
	bool m_use;
};

#endif