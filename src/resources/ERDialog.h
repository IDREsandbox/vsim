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

class ERDialog : public QDialog {
	Q_OBJECT
public:
	ERDialog(QAbstractItemModel *category_model, EditDeleteDelegate *category_editor, QWidget *parent = nullptr);

	// only call this once?
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

	ECategory *getCategory() const;

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
	const ECategoryGroup* m_categories;

	QAbstractItemModel *m_category_model;
	NewCatDialog *m_new_cat_dialog;
	QListView *m_category_view;
	EditDeleteDelegate *m_category_delegate;
};

#endif // ERDIALOG_HPP
