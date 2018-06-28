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
	ERDialog(ECategoryControl *category_control, QString current_dir, QWidget *parent = nullptr);

	void init(const EResource *er);
	void showPositionButton(bool show = true); // default is off

	std::string getTitle() const;
	std::string getDescription() const;
	std::string getAuthor() const;
	std::string getPath() const;
	EResource::Copyright getCopyright() const;
	int getMinYear() const;
	int getMaxYear() const;
	bool getGlobal() const;
	bool getReposition() const;
	//bool getAutoLaunch() const;
	EResource::AutoLaunch getAutoLaunch() const;
	float getLocalRange() const;
	EResource::ERType getERType() const;

	ECategory *categoryAt(int index) const;
	ECategory *getCategory() const;
	std::shared_ptr<ECategory> categoryShared() const;
	void setCategory(const ECategory *cat);

	void chooseFile();

	// the relative thing is really confusing
	void setPath(const QString &s); // use on load, or text edit
	void setRelative(bool relative); // use on checkbox change
	void checkRelative();
	void checkAutoLaunch();

	void setGlobal(bool global);

	// Choose button depends on type
	// URL bar depends on type
	void onTypeChange();

	// Radius depends on global/local
	void onActivationChange();

signals:
	void sNewCategory();
	void sEditCategory(ECategory *category);
	void sDeleteCategory(ECategory *category);
	void sSetPosition();

private:
	Ui::ERDialog ui;
	QString m_current_dir;
	//QString m_file_path; // relative or absolute
	//QString m_url;

	QListView *m_category_view;
	EditDeleteDelegate *m_category_delegate;
	QPushButton *m_position_button;

	ECategoryControl *m_control;
};

#endif // ERDIALOG_HPP
