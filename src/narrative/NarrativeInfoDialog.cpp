#include <QDebug>
#include "NarrativeInfoDialog.h"

NarrativeInfoDialog::NarrativeInfoDialog(QWidget * parent) 
	: QDialog(parent) 
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	// if you don't want the icon in the top left, also removes the X button
	//setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
}

NarrativeInfoDialog::NarrativeInfoDialog(const Narrative2 *nar, QWidget *parent)
	: NarrativeInfoDialog(parent)
{
	ui.editTitle->setText(QString::fromStdString(nar->getTitle()));
	ui.editDescription->setText(QString::fromStdString(nar->getDescription()));
	ui.editContact->setText(QString::fromStdString(nar->getAuthor()));
}

NarrativeInfoDialog::~NarrativeInfoDialog() {
}

std::string NarrativeInfoDialog::getTitle() const
{
	return ui.editTitle->text().toStdString();
}

std::string NarrativeInfoDialog::getDescription() const
{
	return ui.editDescription->toPlainText().toStdString();
}

std::string NarrativeInfoDialog::getAuthor() const
{
	return ui.editContact->toPlainText().toStdString();
}
