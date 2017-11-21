#pragma once
#ifndef ERDIALOG_HPP
#define ERDIALOG_HPP
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_ERDialog.h"
#include "EResource.h"

class ECategoryGroup;

class ERDialog : public QDialog {
	Q_OBJECT
public:
	ERDialog(const EResource *er, const ECategoryGroup *categories, QWidget *parent = nullptr);
	~ERDialog();

	std::string getTitle() const;
	std::string getDescription() const;
	std::string getAuthor() const;
	std::string getPath() const;
	EResource::Copyright getCopyright() const;
	int getMinYear() const;
	int getMaxYear() const;
	bool getGlobal() const;
	bool getReposition() const;
	bool getAutoLaunch() const;
	float getLocalRange() const;
	EResource::ERType getERType() const;

	int getCategory() const;

	void chooseFile();

signals:
	void addNewCat();

private:
	Ui::ERDialog ui;
	const ECategoryGroup* m_categories;
};

#endif // ERDIALOG_HPP
