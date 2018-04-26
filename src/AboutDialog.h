#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_AboutDialog.h"

class AboutDialog : public QDialog {
	Q_OBJECT
public:
	AboutDialog(QWidget *parent = nullptr);


private:
	Ui::AboutDialog ui;
};

#endif