#pragma once
#ifndef NEWCATDOALOG_HPP
#define NEWCATDIALOG_HPP
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_NewCatDialog.h"
#include "resources/EResource.h"

class NewCatDialog : public QDialog {
	Q_OBJECT
public:
	NewCatDialog(QWidget *parent = nullptr);
	~NewCatDialog();

	std::string getCatTitle() const;
	int getRed() const;
	int getGreen()const;
	int getBlue()const;

public slots:
	void pickColor();

private:
	void setGui();
	QColor color;
	Ui::NewCatDialog ui;
};

#endif // NEWCATDIALOG_HPP