#include "LockDialog.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

#include "VSimRoot.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "Gui/PasswordDialog.h"


LockDialog::LockDialog(QWidget * parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);

	connect(ui.edit_button, &QAbstractButton::pressed, this, &LockDialog::execUnlockThis);

	checkEnablePassword();
	connect(ui.change_password, &QCheckBox::stateChanged, this, &LockDialog::checkEnablePassword);

	connect(ui.button_box, &QDialogButtonBox::accepted, this, &LockDialog::tryAccept);
	connect(ui.button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void LockDialog::execEdit(VSimRoot *root)
{
	m_root = root;

	// copy over the lock table
	m_this_lock = *root->lockTableConst();
	bool was_locked = m_this_lock.isLocked();
	m_was_locked = was_locked;
	bool has_pw = m_this_lock.hasPassword();

	bool hard_locked = was_locked && !has_pw;
	bool pw_locked = was_locked && has_pw;

	if (hard_locked) {
		ui.meta_label->setText("This file is locked.");
	}

	if (pw_locked) {
		ui.meta_label->setText("This file is locked with a password.");
	}

	ui.edit_button->setVisible(pw_locked);

	if (was_locked) {
		ui.lock_group_box->setChecked(was_locked);
		ui.lock_settings->setChecked(root->settingsLocked());
		ui.lock_restrict->setChecked(root->restrictedToCurrent());
		ui.lock_navigation->setChecked(root->navigationLocked());
	}
	else {
		// default
		ui.lock_all->setChecked(true);
		ui.lock_settings->setChecked(true);
		ui.lock_restrict->setChecked(true);
		ui.lock_navigation->setChecked(false);
		ui.expiration_checkbox->setChecked(false);
	}

	ui.change_password->setVisible(pw_locked);
	ui.change_password->setChecked(false);
	ui.use_password->setVisible(!hard_locked);
	ui.use_password->setChecked(true);
	ui.embed_models->setChecked(false);
	ui.embed_models->setEnabled(!root->settingsLocked());
	ui.lock_all->setVisible(!hard_locked);
	ui.meta_label->setVisible(was_locked);
	ui.lock_group_box->setEnabled(!was_locked);
	checkEnablePassword();

	// TODO: init expiration date

	this->adjustSize();

	// exec() - see tryAccept();
	int result = exec();

	if (result == QDialog::Rejected) {
		return;
	}


	std::string new_password;

	// can't do anything,
	// maybe embed all models?
	if (m_this_lock.isLocked()) {
		//return;
		return;
	}

	bool lock = ui.lock_group_box->isChecked();
	bool new_lock_pw = lock && !was_locked && ui.use_password->isChecked();
	bool new_lock_perm = lock && !was_locked && !ui.use_password->isChecked();
	bool change_pw = lock && was_locked && ui.use_password->isChecked()
		&& ui.change_password->isChecked();
	bool change_pw_perm = lock && was_locked && !ui.use_password->isChecked();
	bool unlock = !lock && was_locked;
	QString pw = ui.password->text();

	bool pw_lock_ok = false;
	int nar_lock_ok = 0;
	int nar_lock_fail = 0;
	int er_lock_ok = 0;
	int er_lock_fail = 0;
	int models_embedded = 0;

	if (new_lock_pw || change_pw) {
		// generate a hash
		// apply that hash to this, nars, etc
		HashLock hash = HashLock::generateHash(pw.toStdString());
		m_this_lock.lockWithPasswordHash(hash);
		root->setLockTable(m_this_lock);

		// TODO: lock nars
		//root->narratives()->lock
		//root->narratives()->locks();
		// mass lock with hash
		if (ui.lock_all->isChecked()) {
			qDebug() << "new lock with password";
			for (auto nar : *root->narratives()) {
				if (nar->lockTable()->lockWithPasswordHash(hash)) {
					nar_lock_ok++;
				}
				else {
					nar_lock_fail++;
				}
			}
		}
	}

	if (new_lock_perm || change_pw_perm) {
		m_this_lock.lock();
		root->setLockTable(m_this_lock);

		if (ui.lock_all->isChecked()) {
			for (auto nar : *root->narratives()) {
				if (nar->lockTable()->lock()) {
					nar_lock_ok++;
				}
				else {
					nar_lock_fail++;
				}
			}
		}

		// mass lock permanent
	}

	if (lock) {
		root->setSettingsLocked(ui.lock_settings->isChecked());
		root->setRestrictToCurrent(ui.lock_restrict->isChecked());
		root->setNavigationLocked(ui.lock_navigation->isChecked());
		// don't reassign lock if nothing changed
	}

	if (unlock) {
		root->setSettingsLocked(false);
		root->setRestrictToCurrent(false);
		root->setNavigationLocked(false);

		root->setLockTable(m_this_lock);
	}

	// output message

	QString msg;

	if (new_lock_pw) {
		msg += "File was locked with a password.";
	}
	if (new_lock_perm || change_pw_perm) {
		msg += "File was permanently locked.";
	}
	if (change_pw) {
		msg += "Password was changed.";
	}
	if (unlock) {
		msg += "File was unlocked.";
	}

	if (nar_lock_ok > 0) msg += QString("\nLocked %1 narratives.").arg(nar_lock_ok);
	if (nar_lock_fail > 0) msg += QString("\nFailed to lock %1 narratives.").arg(nar_lock_fail);
	if (er_lock_ok > 0) msg += QString("\nLocked %1 resources.").arg(er_lock_ok);
	if (er_lock_fail > 0) msg += QString("\nFailed to lock %1 resources.").arg(er_lock_fail);
	if (models_embedded > 0) msg += QString("\nEmbedded %1 models.").arg(models_embedded);

	if (!msg.isEmpty()) {
		QMessageBox::information(nullptr, "Lock Settings", msg);
	}

	// TODO: clear undo history?
}

void LockDialog::execUnlockThis()
{
	// only valid when we have a password to lock
	//if (!(m_root->locked() && m_root->hasPassword())) return;

	if (!(m_this_lock.isLocked() && m_this_lock.hasPassword())) return;

	bool pw_ok = false;

	TestPasswordDialog dlg;
	dlg.setWindowTitle("Edit Lock Settings");
	dlg.setLabel("Enter password:");
	dlg.showSkipButton(false);
	dlg.setTestCallback([&](QString pw) -> bool {
		// test password
		bool success = m_this_lock.unlock(pw);

		// remember this password
		m_this_password = pw;

		return success;
	});

	int result = dlg.exec();
	if (result == QDialog::Rejected) return;

	// unlock stuff
	ui.lock_group_box->setEnabled(true);
	ui.embed_models->setEnabled(true);
	ui.edit_button->hide();
	adjustSize();
}

void LockDialog::tryAccept()
{
	// do warnings
	// - permanent warning
	// - new lock warning

	// no warnings
	// - was already permanent
	// - otherwise

	bool new_lock = !m_was_locked
		&& !m_this_lock.isPermanent()
		&& ui.lock_group_box->isChecked();

	bool perm_lock = ui.lock_group_box->isChecked()
		&& !m_this_lock.isPermanent()
		&& !ui.use_password->isChecked();

	auto doWarning = [&](QString text) -> bool {
		int result = QMessageBox::warning(this, "Lock Settings", text,
			QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel);
		bool ok = (result == QMessageBox::StandardButton::Ok);
		return ok;
	};

	QString msg;
	if (new_lock) {
		msg = "Are you sure you want to lock this file?";
	}

	if (perm_lock) {
		if (doWarning("Are you sure you want to permanently lock this file?")) {
			accept();
		}
		return;
	}
	else if (new_lock) {
		if (doWarning("Are you sure you want to lock this file?")) {
			accept();
		}
		return;
	}
	else {
		accept();
		return;
	}
}

void LockDialog::checkEnablePassword()
{
	bool enabled = true;
	// wasn't previously locked? then ignore the change password thing
	if (m_was_locked) {
		enabled = enabled & ui.change_password->isChecked();
	}
	ui.password->setEnabled(enabled);
}
