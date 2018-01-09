#include <QDebug>
#include <string>
#include <QListView>
#include <QFileDialog>
#include <QMessageBox>
#include "resources/ERDialog.h"
#include "resources/NewCatDialog.h"
#include "resources/EResource.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"
#include "EditDeleteDelegate.h"
#include "GroupModel.h"

ERDialog::ERDialog(QAbstractItemModel *category_model, EditDeleteDelegate *category_editor, QWidget * parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	for (uint i = 0; i < sizeof(EResource::CopyrightStrings) / sizeof(char*); i++) {
		ui.licensing->addItem(EResource::CopyrightStrings[i]);
	}
	ui.licensing->setCurrentIndex(EResource::UNSPECIFIED);

	init(nullptr);

	connect(ui.choose, &QPushButton::clicked, this, &ERDialog::chooseFile);

	// only one call to onTypeChange occurs when toggles happen
	auto ontoggletype = [this](bool x) { if (x) this->onTypeChange(); };
	connect(ui.url, &QRadioButton::toggled, this, ontoggletype);
	connect(ui.file, &QRadioButton::toggled, this, ontoggletype);
	connect(ui.annotation, &QRadioButton::toggled, this, ontoggletype);

	auto ontoggleactive = [this](bool x) { if (x) this->onActivationChange(); };
	connect(ui.global, &QRadioButton::toggled, this, ontoggleactive);
	connect(ui.local, &QRadioButton::toggled, this, ontoggleactive);

	onActivationChange();
	onTypeChange();

	ui.year_lower->setValue(0);
	ui.year_upper->setValue(0);
	ui.annotation->setChecked(true);
	ui.radius->setValue(10);

	// category stuff
	m_category_view = new QListView(this);
	m_category_view->setModel(category_model);
	m_category_delegate = category_editor;
	m_category_view->setItemDelegate(category_editor);
	ui.categories->setMouseTracking(true);
	ui.categories->setModel(category_model);
	ui.categories->setView(m_category_view);

	connect(ui.addnew, &QPushButton::pressed, this, &ERDialog::sNewCategory);
}

void ERDialog::setCategoryGroup(const ECategoryGroup * categories)
{
	m_categories = categories;
}

void ERDialog::init(const EResource * er)
{
	if (er == nullptr) { // Defaults
		ui.title->setText("Untitled");
		ui.description->setText("");
		ui.authors->setText("");
		ui.authors->setText("");
		// make all of the other fields just copy the previous?
		return;
	}

	ui.title->setText(QString::fromStdString(er->getResourceName()));
	ui.description->setText(QString::fromStdString(er->getResourceDescription()));
	ui.authors->setText(QString::fromStdString(er->getAuthor()));
	ui.path->setText(QString::fromStdString(er->getResourcePath()));
	ui.year_lower->setValue(er->getMinYear());
	ui.year_upper->setValue(er->getMaxYear());
	ui.radius->setValue(er->getLocalRange());

	if (er->getGlobal()) ui.global->setChecked(true);
	else ui.local->setChecked(true);

	ui.autolaunch->setChecked(er->getAutoLaunch());

	switch (er->getERType()) {
	case EResource::FILE:
		ui.file->setChecked(true);
		break;
	case EResource::ANNOTATION:
		ui.annotation->setChecked(true);
		break;
	case EResource::URL:
		ui.url->setChecked(true);
		break;
	}
}

std::string ERDialog::getTitle() const
{
	return ui.title->text().toStdString();
}

std::string ERDialog::getDescription() const
{
	return ui.description->toPlainText().toStdString();
}

std::string ERDialog::getAuthor() const
{
	return ui.authors->toPlainText().toStdString();
}

std::string ERDialog::getPath() const
{
	if (ui.annotation->isChecked()) {
		return "";
	}
	return ui.path->text().toStdString();
}

EResource::Copyright ERDialog::getCopyright() const
{
	return (EResource::Copyright)ui.licensing->currentIndex();
}

int ERDialog::getMinYear() const
{
	return ui.year_lower->value();
}

int ERDialog::getMaxYear() const
{
	return ui.year_upper->value();
}

bool ERDialog::getGlobal() const
{
	return ui.global->isChecked();
}

bool ERDialog::getReposition() const
{
	return ui.on->isChecked();
}

bool ERDialog::getAutoLaunch() const
{
	return ui.on_2->isChecked() && getERType() != EResource::ANNOTATION;
}

float ERDialog::getLocalRange() const
{
	if (ui.global->isChecked()) {
		return 0;
	}
	return ui.radius->value();
}

EResource::ERType ERDialog::getERType() const
{
	if (ui.file->isChecked()) {
		return EResource::FILE;
	}
	else if (ui.annotation->isChecked()) {
		return EResource::ANNOTATION;
	}
	else {
		return EResource::URL;
	}
}

int ERDialog::getCategory() const
{
	return ui.categories->currentIndex();
}

void ERDialog::chooseFile()
{
	QFileDialog dlg;

	ui.path->setText(dlg.getOpenFileName());
}

void ERDialog::onTypeChange()
{
	if (ui.file->isChecked()) {
		ui.path->setEnabled(true);
		ui.choose->setEnabled(true);
	}
	else if (ui.annotation->isChecked()) {
		ui.path->setEnabled(false);
		ui.choose->setEnabled(false);
	}
	else if (ui.url->isChecked()) {
		ui.path->setEnabled(true);
		ui.choose->setEnabled(false);
	}
}

void ERDialog::onActivationChange()
{
	if (ui.global->isChecked()) {
		ui.radius->setEnabled(false);
	}
	else if (ui.local->isChecked()) {
		ui.radius->setEnabled(true);
	}
}
