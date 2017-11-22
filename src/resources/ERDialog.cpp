#include <QDebug>
#include <string>
#include <qfiledialog.h>
#include "resources/ERDialog.h"
#include "resources/NewCatDialog.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"

ERDialog::ERDialog(const EResource *er, const ECategoryGroup *categories, QWidget * parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_categories = categories;

	for (uint i = 0; i < sizeof(EResource::CopyrightStrings) / sizeof(char*); i++) {
		ui.licensing->addItem(EResource::CopyrightStrings[i]);
	}
	ui.licensing->setCurrentIndex(EResource::UNSPECIFIED);

	if (er == nullptr) { // Defaults
		ui.year_lower->setValue(0);
		ui.year_upper->setValue(0);
	}
	else {
		ui.title->setText(QString::fromStdString(er->getResourceName()));
		ui.description->setText(QString::fromStdString(er->getResourceDescription()));
		ui.authors->setText(QString::fromStdString(er->getAuthor()));
		ui.path->setText(QString::fromStdString(er->getResourcePath()));
		ui.year_lower->setValue(er->getMinYear());
		ui.year_upper->setValue(er->getMaxYear());
		ui.radius->setValue(er->getLocalRange());

		ui.global->setChecked(er->getGlobal());
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

	connect(ui.addnew, &QPushButton::clicked, this, &ERDialog::addNewCat);
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
}

ERDialog::~ERDialog() {
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
