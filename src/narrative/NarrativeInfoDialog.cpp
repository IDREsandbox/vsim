#include <QDebug>
#include "NarrativeInfoDialog.h"

NarrativeInfoDialog::NarrativeInfoDialog(QWidget * parent) : QDialog(parent) {
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	// if you don't want the icon in the top left, also removes the X button
	//setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
}

NarrativeInfoDialog::NarrativeInfoDialog(QWidget * parent, const NarrativeInfo &info)
	: NarrativeInfoDialog(parent)
{
	ui.editTitle->setText(QString::fromStdString(info.m_title));
	ui.editDescription->setText(QString::fromStdString(info.m_description));
	ui.editContact->setText(QString::fromStdString(info.m_contact));
	m_result = info;
}

NarrativeInfoDialog::~NarrativeInfoDialog() {
}

const NarrativeInfo & NarrativeInfoDialog::getInfo()
{	
	return m_result;
}

void NarrativeInfoDialog::accept()
{
	m_result = {
		ui.editTitle->text().toStdString(),
		ui.editDescription->toPlainText().toStdString(),
		ui.editContact->toPlainText().toStdString()
	};
	QDialog::accept();
}


