#pragma once
#ifndef ERDIALOG_HPP
#define ERDIALOG_HPP
#include <QDialog>
#include <QDialogButtonBox>

#include "resources/EResource.h"
#include "ui_ERDialog.h"

class EResource;
class ECategoryGroup;

class ERDialog : public QDialog {
	Q_OBJECT
public:
	ERDialog(QWidget *parent = nullptr);
	~ERDialog();

	void setCategoryGroup(const ECategoryGroup *categories);
	void init(const EResource *er);

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

	// Choose button depends on type
	// URL bar depends on type
	void onTypeChange();

	// Radius depends on global/local
	void onActivationChange();

signals:
	void addNewCat();

private:
	Ui::ERDialog ui;
	const ECategoryGroup* m_categories;
};

#endif // ERDIALOG_HPP
