#ifndef ERDIALOG_HPP
#define ERDIALOG_HPP
#include <QDialog>
#include <QDialogButtonBox>
#include <QListView>
#include <QAbstractItemModel>

#include "resources/EResource.h"
#include "ui_ERDialog.h"

class EResource;
class ECategoryGroup;
class EditDeleteDelegate;
class ECategory;
class NewCatDialog;
class ECategoryControl;

class ERDialog : public QDialog {
	Q_OBJECT
public:
	ERDialog(ECategoryControl *category_control, QWidget *parent = nullptr);

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

	ECategory *categoryAt(int index) const;
	ECategory *getCategory() const;
	void setCategory(const ECategory *cat);

	void chooseFile();

	// Choose button depends on type
	// URL bar depends on type
	void onTypeChange();

	// Radius depends on global/local
	void onActivationChange();

signals:
	void sNewCategory();
	void sEditCategory(ECategory *category);
	void sDeleteCategory(ECategory *category);

private:
	Ui::ERDialog ui;

	QListView *m_category_view;
	EditDeleteDelegate *m_category_delegate;

	ECategoryControl *m_control;
};

#endif // ERDIALOG_HPP
