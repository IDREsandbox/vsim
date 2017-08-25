#include <QDebug>
#include "resources/ERDialog.h"
#include "resources/NewCatDialog.h"

ERDialog::ERDialog(QWidget * parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	// if you don't want the icon in the top left, also removes the X button
	//setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

	ui.licensing->addItem("Copyrighted Resource");
	ui.licensing->addItem("Fair Use");
	ui.licensing->addItem("Held by Creator");
	ui.licensing->addItem("Public Domain");
	ui.licensing->addItem("Unknown Source");
	ui.licensing->addItem("Used with Permission");
	ui.licensing->addItem("Web Resource");
	ui.licensing->setCurrentIndex(0);

	connect(ui.addnew, &QPushButton::clicked, this, &ERDialog::addNewCat);
}

ERDialog::ERDialog(const EResource *er, QWidget *parent)
	: ERDialog(parent)
{
	ui.title->setText(QString::fromStdString(er->getResourceName()));
	ui.description->setText(QString::fromStdString(er->getResourceDescription()));
	ui.authors->setText(QString::fromStdString(er->getAuthor()));
	ui.path->setText(QString::fromStdString(er->getResourcePath()));
	ui.lower->setText(QString::fromStdString(std::to_string(er->getMinYear())));
	ui.upper->setText(QString::fromStdString(std::to_string(er->getMaxYear())));
	ui.radius->setValue(er->getLocalRange());

	ui.licensing->addItem("Copyrighted Resource");
	ui.licensing->addItem("Fair Use");
	ui.licensing->addItem("Held by Creator");
	ui.licensing->addItem("Public Domain");
	ui.licensing->addItem("Unknown Source");
	ui.licensing->addItem("Used with Permission");
	ui.licensing->addItem("Web Resource");
	ui.licensing->setCurrentIndex(er->getCopyRight());

	connect(ui.addnew, &QPushButton::clicked, this, &ERDialog::addNewCat);
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

void ERDialog::addNewCat()
{
	NewCatDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	ui.categories->addItem(QString::fromStdString(dlg.getCatTitle()));
}

void ERDialog::chooseFile()
{

}
