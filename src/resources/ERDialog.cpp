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

		if (er->getGlobal())
			ui.global->setChecked(true);
		else
			ui.local->setChecked(true);
	}

	connect(ui.addnew, &QPushButton::clicked, this, &ERDialog::addNewCat);
	connect(ui.choose, &QPushButton::clicked, this, &ERDialog::chooseFile);
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
	return ui.radius->value();
}

EResource::ERType ERDialog::getERType() const
{
	if (ui.file->isChecked())
		return EResource::FILE;
	else if (ui.annotation->isChecked())
		return EResource::ANNOTATION;
	else
		return EResource::URL;
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
