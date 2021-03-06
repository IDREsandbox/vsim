﻿#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include "ui_HelpDialog.h"

#include <QDialog>

class HelpDialog : public QDialog {
	Q_OBJECT;
public:
	HelpDialog(QWidget *parent = nullptr);

	void reload();
	void loadHelpFile(QString html_path, QString hotkeys_path);
	void loadHelp(QString html, QString hotkeys);

private:
	QString readFile(QString s);

private:
	Ui::HelpDialog ui;
};

#endif