#pragma once
#ifndef ERDIALOG_HPP
#define ERDIALOG_HPP
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_ERDialog.h"
#include "resources/EResource.h"

class ERDialog : public QDialog {
	Q_OBJECT
public:
	ERDialog(QWidget *parent = nullptr);
	ERDialog(const EResource *er, QWidget *parent = nullptr);
	~ERDialog();

	std::string getTitle() const;
	std::string getDescription() const;
	std::string getAuthor() const;
	std::string getPath() const;
	int getCopyRight() const;
	int getMinYear() const;
	int getMaxYear() const;
	int getGlobal() const;
	int getReposition() const;
	int getAutoLaunch() const;
	float getLocalRange() const;
	int getErType() const;

	void addNewCat();
	void chooseFile();

private:
	void setGui();
	Ui::ERDialog ui;
};

#endif // ERDIALOG_HPP
