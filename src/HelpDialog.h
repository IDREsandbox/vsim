#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include "ui_HelpDialog.h"

#include <QDialog>

class HelpDialog : public QDialog {
	Q_OBJECT;
public:
	HelpDialog(QWidget *parent = nullptr);

private:
	Ui::HelpDialog ui;
};

#endif