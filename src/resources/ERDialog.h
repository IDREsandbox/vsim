#pragma once
#ifndef ERDIALOG_HPP
#define ERDIALOG_HPP
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_ERDialog.h"
#include "resources/EResource.h"
#include "MainWindow.h"

class ERDialog : public QDialog {
	Q_OBJECT
public:
	ERDialog(MainWindow* mw, QWidget *parent = nullptr);
	ERDialog(const EResource *er, MainWindow* mw, QWidget *parent = nullptr);
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

	int getCategory() const;

	void addNewCat();
	void chooseFile();

private:
	void setGui();
	MainWindow* main;
	Ui::ERDialog ui;
};

#endif // ERDIALOG_HPP
