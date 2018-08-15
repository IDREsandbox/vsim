#include "LockDialog.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

#include "VSimRoot.h"

LockDialog::LockDialog(QWidget * parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);

	connect(ui.edit_button, &QAbstractButton::pressed, this, [this]() {
		// only valid when we have a password to lock
		if (!(m_root->locked() && m_root->hasPassword())) return;

		bool pw_ok = false;

		while (!pw_ok) {
			// password dialog, check password
			bool ok;
			QString pw = QInputDialog::getText(this,
				"Edit Lock Settings",
				"Enter password:",
				QLineEdit::EchoMode::Password, "", &ok);

			if (!ok) {
				return;
			}

			// try to unlock
			pw_ok = m_root->checkPassword(pw.toStdString());

			if (!pw_ok) {
				QMessageBox::warning(this, "Edit Lock Settings", "Incorrect password.");
			}
		}

		// unlock stuff
		ui.lock_group_box->setEnabled(true);
		ui.embed_models->setEnabled(true);
		ui.edit_button->hide();
		m_this_locked = false;
	});

	checkEnablePassword();
	checkEnableExpiration();
	connect(ui.password_checkbox, &QCheckBox::stateChanged, this, &LockDialog::checkEnablePassword);
	connect(ui.expiration_checkbox, &QCheckBox::stateChanged, this, &LockDialog::checkEnableExpiration);

	connect(ui.button_box, &QDialogButtonBox::accepted, this, &LockDialog::tryAccept);
	connect(ui.button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void LockDialog::execEdit(VSimRoot *root)
{
	m_root = root;
	bool was_locked = root->locked();
	bool has_pw = root->hasPassword();

	bool hard_locked = was_locked && !has_pw;
	bool pw_locked = was_locked && has_pw;

	if (hard_locked) {
		ui.meta_label->setText("This file is locked.");
		ui.lock_group_box->setEnabled(false);
	}

	if (pw_locked) {
		ui.meta_label->setText("This file is locked with a password.");
		ui.password_checkbox->setText("Change password");
	}

	ui.edit_button->setVisible(pw_locked);

	if (was_locked) {
		ui.lock_group_box->setChecked(was_locked);
		ui.lock_settings->setChecked(root->settingsLocked());
		ui.lock_restrict->setChecked(root->restrictedToCurrent());
		ui.lock_navigation->setChecked(root->navigationLocked());
	}
	else {
		ui.password_checkbox->setText("Password");
		// default
		ui.lock_all->setChecked(true);
		ui.lock_settings->setChecked(true);
		ui.lock_restrict->setChecked(true);
		ui.lock_navigation->setChecked(false);
	}

	ui.password_checkbox->setChecked(!was_locked);
	ui.password_checkbox->setVisible(!hard_locked);
	ui.password->setVisible(!hard_locked);
	ui.embed_models->setChecked(false);
	ui.embed_models->setEnabled(!root->settingsLocked());
	ui.lock_all->setVisible(!hard_locked);
	ui.meta_label->setVisible(was_locked);
	ui.lock_group_box->setEnabled(!was_locked);
	m_this_locked = was_locked;

	// TODO: init expiration date

	this->adjustSize();

	int result = exec();

	if (result == QDialog::Rejected) {
		return;
	}


	std::string new_password;

	// can't do anything,
	// maybe embed all models?
	if (m_this_locked) {
		//return;
	}

	if (!m_this_locked) {
		// we can apply changes

		if (ui.lock_group_box->isChecked()) {
			std::string pw = ui.password->text().toStdString();
			// change password
			// or lock w/ password
			if (ui.password_checkbox->isChecked() && !pw.empty()) {
				root->lockWithPassword(pw);
			}
			// re-lock
			else if (!was_locked) {
				root->lock();
			}

			// other stuff
			root->setSettingsLocked(ui.lock_settings->isChecked());
			root->setRestrictToCurrent(ui.lock_restrict->isChecked());
			root->setNavigationLocked(ui.lock_navigation->isChecked());

			// nars and resources? TODO
		}
		else {
			root->unlock();
		}
	}
}

void LockDialog::tryAccept()
{
	// check password
	// do are you sure stuff
	if (m_this_locked) {
		accept();
		return; // TODO: model stuff
	}

	bool do_check = ui.lock_group_box->isChecked() && !m_root->locked();

	if (!do_check) {
		accept();
		return;
	}

	std::string pw = ui.password->text().toStdString();

	bool empty_warning = pw.empty();

	QString warning = "Are you sure you want to lock this file?";
	if (pw.empty()) warning += " A locked file without a password cannot be unlocked.";

	int result = QMessageBox::warning(this, "Lock Settings", warning,
		QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel);

	if (result == QMessageBox::StandardButton::Cancel) {
		return;
	}

	accept();
}

void LockDialog::checkEnablePassword()
{
	ui.password->setEnabled(ui.password_checkbox->isChecked());
}

void LockDialog::checkEnableExpiration()
{
	ui.expiration_date->setEnabled(ui.expiration_checkbox->isChecked());
}
