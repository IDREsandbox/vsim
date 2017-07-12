#include <QDebug>
#include "dragLabelInput.h"

dragLabelInput::dragLabelInput(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	// if you don't want the icon in the top left, also removes the X button
	//setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
}

dragLabelInput::dragLabelInput(QWidget* parent, const QString &text)
	: dragLabelInput(parent)
{
	ui.textEdit->setText(text);
	m_result = text;
}

dragLabelInput::~dragLabelInput() {
}

const QString & dragLabelInput::getInfo()
{
	return m_result;
}

void dragLabelInput::accept()
{
	m_result = ui.textEdit->toHtml();
	QDialog::accept();
}

