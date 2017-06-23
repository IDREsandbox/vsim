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
	m_info = info;
	setGui();
}

NarrativeInfoDialog::~NarrativeInfoDialog() {
}

void NarrativeInfoDialog::setInfo(const Narrative &nar)
{
	m_info = {
		nar.getName(),
		nar.getDescription(),
		nar.getAuthor()
	};
	setGui();
}

const NarrativeInfo & NarrativeInfoDialog::getInfo()
{	
	return m_info;
}

void NarrativeInfoDialog::clear()
{
	m_info = {};
	setGui();
}

void NarrativeInfoDialog::accept()
{
	m_info = {
		ui.editTitle->text().toStdString(),
		ui.editDescription->toPlainText().toStdString(),
		ui.editContact->toPlainText().toStdString()
	};
	QDialog::accept();
}

void NarrativeInfoDialog::setGui()
{
	ui.editTitle->setText(QString::fromStdString(m_info.m_title));
	ui.editDescription->setText(QString::fromStdString(m_info.m_description));
	ui.editContact->setText(QString::fromStdString(m_info.m_contact));
}


