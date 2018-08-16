#include "PasswordDialog.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

CreatePasswordDialog::CreatePasswordDialog(QWidget *parent)
	: QDialog(parent),
	m_use(true)
{
	ui.setupUi(this);

	setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);

	onUseChange();

	connect(ui.password_checkbox, &QAbstractButton::pressed, this,
		[this]() {
		m_use = !m_use;
		onUseChange();
	});

	connect(ui.button_box, &QDialogButtonBox::rejected,
		this, &QDialog::reject);
	connect(ui.button_box, &QDialogButtonBox::accepted,
		this, &TestPasswordDialog::accept);
}

void CreatePasswordDialog::onUseChange() {
	ui.password_checkbox->setChecked(m_use);
	ui.password->setEnabled(m_use);
}

bool CreatePasswordDialog::usePassword() const
{
	return m_use;
}

QString CreatePasswordDialog::password() const
{
	if (!m_use) return "";
	return ui.password->text();
}

void CreatePasswordDialog::setLabel(QString text)
{
	if (text.isEmpty()) {
		ui.label->hide();
		ui.label->setText("");
	}
	else {
		ui.label->show();
		ui.label->setText(text);
	}
	adjustSize();
}

//QString CreatePasswordDialog::getPasswordCheckable(bool *ok, bool *no_pw)
//{
//	PasswordDialog dlg;
//	dlg.setUsePassword(true);
//	int result = dlg.exec();
//
//	if (result == QDialog::Rejected) {
//		*ok = false;
//		return "";
//	}
//	*ok = true;
//	if (!dlg.usePassword()) {
//		*no_pw = true;
//		return "";
//	}
//	*no_pw = false;
//	return dlg.ui.password->text();
//}

//QString CreatePasswordDialog::getPassword(bool *ok, QString title, QString label)
//{
//	return QInputDialog::getText(nullptr, title, label, QLineEdit::Password, "", ok,
//		Qt::WindowType::Dialog);
//}

TestPasswordDialog::TestPasswordDialog(QWidget * parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);

	ui.password_checkbox->hide();

	m_skip_button
		= ui.button_box->addButton("Skip", QDialogButtonBox::ButtonRole::NoRole);
	m_skip_button->hide();

	adjustSize();

	connect(ui.button_box, &QDialogButtonBox::rejected,
		this, &QDialog::reject);
	connect(ui.button_box, &QDialogButtonBox::accepted,
		this, &TestPasswordDialog::tryAccept);
	connect(m_skip_button, &QAbstractButton::pressed,
		this, &QDialog::accept);
}

void TestPasswordDialog::setLabel(QString text)
{
	ui.label->setText(text);
}

void TestPasswordDialog::showSkipButton(bool show) {
	m_skip_button->setVisible(show);
}

void TestPasswordDialog::setTestCallback(std::function<bool(QString)> f)
{
	m_func = f;
}

void TestPasswordDialog::tryAccept()
{
	bool ok = m_func(ui.password->text());
	if (ok) {
		accept();
	}
	else {
		QMessageBox::warning(this, this->windowTitle(), "Incorrect password.");
	}
}
