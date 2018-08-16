#include "PasswordDialog.h"

#include <QInputDialog>

PasswordDialog::PasswordDialog(QWidget *parent)
	: QDialog(parent),
	m_use(true)
{
	ui.setupUi(this);

	setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);

	onUseChange(true);
	showLabel(false);

	connect(ui.password_checkbox, &QAbstractButton::pressed, this,
		[this]() {
		onUseChange(!m_use);
	});
}

void PasswordDialog::onUseChange(bool use) {
	ui.password_checkbox->setChecked(use);
	ui.password->setEnabled(use);
}

void PasswordDialog::setUsePassword(bool use)
{
	m_use = use;
	onUseChange(use);
}

bool PasswordDialog::usePassword() const
{
	return m_use;
}

QString PasswordDialog::password() const
{
	if (!m_use) return "";
	return ui.password->text();
}

void PasswordDialog::setLabelText(QString text)
{
	ui.label->setText(text);
}

void PasswordDialog::showLabel(bool show)
{
	ui.label->setVisible(show);
	adjustSize();
}

QString PasswordDialog::getPasswordCheckable(bool *ok, bool *no_pw)
{
	PasswordDialog dlg;
	dlg.setUsePassword(true);
	int result = dlg.exec();

	if (result == QDialog::Rejected) {
		*ok = false;
		return "";
	}
	*ok = true;
	if (!dlg.usePassword()) {
		*no_pw = true;
		return "";
	}
	*no_pw = false;
	return dlg.ui.password->text();
}

QString PasswordDialog::getPassword(bool *ok, QString title, QString label)
{
	return QInputDialog::getText(nullptr, title, label, QLineEdit::Password, "", ok);
}
