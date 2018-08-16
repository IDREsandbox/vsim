#include "narrative/NarrativeInfoDialog.h"

#include <QDebug>
#include <QPushButton>

NarrativeInfoDialog::NarrativeInfoDialog(QWidget * parent) 
	: QDialog(parent) 
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	// if you don't want the icon in the top left, also removes the X button
	//setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
}

NarrativeInfoDialog::NarrativeInfoDialog(const Narrative *nar, QWidget *parent)
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

void NarrativeInfoDialog::setReadOnly(bool read_only)
{
	ui.editTitle->setReadOnly(read_only);
	ui.editDescription->setReadOnly(read_only);
	ui.editContact->setReadOnly(read_only);

	QPushButton *cancel = ui.buttonBox->button(QDialogButtonBox::Cancel);
	cancel->setVisible(!read_only);
}
