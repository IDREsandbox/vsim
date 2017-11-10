#include <QDebug>
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

	ui.licensing->addItem("Copyrighted Resource");
	ui.licensing->addItem("Fair Use");
	ui.licensing->addItem("Held by Creator");
	ui.licensing->addItem("Public Domain");
	ui.licensing->addItem("Unknown Source");
	ui.licensing->addItem("Used with Permission");
	ui.licensing->addItem("Web Resource");
	ui.licensing->setCurrentIndex(0);

	if (er == nullptr) {
		ui.lower->setText("-9999");
		ui.upper->setText("9999");
	}
	else {
		ui.title->setText(QString::fromStdString(er->getResourceName()));
		ui.description->setText(QString::fromStdString(er->getResourceDescription()));
		ui.authors->setText(QString::fromStdString(er->getAuthor()));
		ui.path->setText(QString::fromStdString(er->getResourcePath()));
		ui.lower->setText(QString::fromStdString(std::to_string(er->getMinYear())));
		ui.upper->setText(QString::fromStdString(std::to_string(er->getMaxYear())));
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
	return ui.title->toHtml().toStdString();
}

std::string ERDialog::getDescription() const
{
	return ui.description->toHtml().toStdString();
}

std::string ERDialog::getAuthor() const
{
	return ui.authors->toHtml().toStdString();
}

std::string ERDialog::getPath() const
{
	return ui.path->text().toStdString();
}

int ERDialog::getCopyRight() const
{
	return ui.licensing->currentIndex();
}

int ERDialog::getMinYear() const
{
	return std::stoi(ui.lower->text().toStdString());
}

int ERDialog::getMaxYear() const
{
	return std::stoi(ui.upper->text().toStdString());
}

int ERDialog::getGlobal() const
{
	return ui.global->isChecked();
}

int ERDialog::getReposition() const
{
	return ui.on->isChecked();
}

int ERDialog::getAutoLaunch() const
{
	if (ui.on_2->isChecked())
		return 1;
	else if (ui.text->isChecked())
		return 2;
	else
		return 0;
}

float ERDialog::getLocalRange() const
{
	return ui.radius->value();
}

int ERDialog::getErType() const
{
	if (ui.url->isChecked())
		return 1;
	else if (ui.annotation->isChecked())
		return 2;
	else
		return 0;
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
