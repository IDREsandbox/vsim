#include "LockDialog.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

#include "VSimRoot.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "resources/EResourceGroup.h"
#include "resources/EResource.h"
#include "Gui/PasswordDialog.h"
#include "Model/ModelGroup.h"

LockDialog::LockDialog(QWidget * parent)
	: QDialog(parent),
	m_lock_applied(false)
{
	ui.setupUi(this);
	setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);

	ui.file_widget->hide();
	checkEnablePassword();

	connect(ui.edit_button, &QAbstractButton::pressed, this, &LockDialog::execUnlockThis);
	connect(ui.change_password, &QCheckBox::stateChanged, this, &LockDialog::checkEnablePassword);

	connect(ui.button_box, &QDialogButtonBox::accepted, this, &LockDialog::tryAccept);
	connect(ui.button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);

	connect(ui.file_button, &QAbstractButton::pressed, this, &LockDialog::onChooseFile);
}

void LockDialog::execEdit(VSimRoot *root)
{
	// TODO: clear undo history?
	m_title = "Lock Settings";
	m_package = false;

	auto result = execInternal(root);
	if (result == QDialog::Rejected) return;
	apply();
	execResultsMessage();
}

QDialog::DialogCode LockDialog::execPackage(VSimRoot *root)
{
	m_title = "Export VSim";
	m_package = true;

	bool file_ok = onChooseFile();
	if (!file_ok) return QDialog::Rejected;
	auto result = execInternal(root);
	if (result == QDialog::Accepted) apply();

	return result;
}

QString LockDialog::filePath() const
{
	return ui.file_text->text();
}

bool LockDialog::lockApplied() const
{
	return m_lock_applied;
}

void LockDialog::execUnlockThis()
{
	// only valid when we have a password to lock
	//if (!(m_root->locked() && m_root->hasPassword())) return;

	if (!(m_this_lock.isLocked() && m_this_lock.hasPassword())) return;

	TestPasswordDialog dlg;
	dlg.setWindowTitle("Edit Lock Settings");
	dlg.setLabel("Enter password:");
	dlg.showSkipButton(false);
	dlg.setLock(&m_this_lock);

	int result = dlg.exec();
	if (result == QDialog::Rejected) return;

	// unlock stuff
	ui.lock_group_box->setEnabled(true);
	ui.embed_models->setEnabled(true);
	ui.edit_button->hide();
	adjustSize();
}

void LockDialog::apply()
{
	std::string new_password;
	bool pw_lock_ok = false;
	m_nar_lock_ok = 0;
	m_nar_lock_fail = 0;
	m_er_lock_ok = 0;
	m_er_lock_fail = 0;
	m_models_embedded = 0;
	m_lock_applied = false;

	// embed models
	if (ui.embed_models->isChecked()) {
		m_models_embedded = m_root->models()->embedAll();
	}

	// can't do anything else
	if (m_this_lock.isLocked()) {
		return;
	}

	if (m_new_lock_pw || m_change_pw) {
		// generate a hash
		// apply that hash to this, nars, etc
		HashLock hash = HashLock::generateHash(m_pw.toStdString());
		m_this_lock.lockWithPasswordHash(hash);
		m_root->setLockTable(m_this_lock);
		m_lock_applied = true;

		// TODO: lock nars
		//root->narratives()->lock
		//root->narratives()->locks();
		// mass lock with hash
		if (ui.lock_all->isChecked()) {
			// mass lock w/ password
			for (auto nar : *m_root->narratives()) {
				if (nar->lockTable()->lockWithPasswordHash(hash)) {
					m_nar_lock_ok++;
				}
				else {
					m_nar_lock_fail++;
				}
			}
			for (auto er : *m_root->resources()) {
				if (er->lockTable()->lockWithPasswordHash(hash)) {
					m_er_lock_ok++;
				}
				else {
					m_er_lock_fail++;
				}
			}
		}
	}

	if (m_new_lock_perm || m_change_pw_perm) {
		m_this_lock.lock();
		m_root->setLockTable(m_this_lock);
		m_lock_applied = true;

		// mass lock permanent
		if (ui.lock_all->isChecked()) {
			for (auto nar : *m_root->narratives()) {
				if (nar->lockTable()->lock()) {
					m_nar_lock_ok++;
				}
				else {
					m_nar_lock_fail++;
				}
			}
			for (auto er : *m_root->resources()) {
				if (er->lockTable()->lock()) {
					m_er_lock_ok++;
				}
				else {
					m_er_lock_fail++;
				}
			}
		}
	}

	if (m_lock) {
		m_root->setSettingsLocked(ui.lock_settings->isChecked());
		m_root->setRestrictToCurrent(ui.lock_restrict->isChecked());
		m_root->setNavigationLocked(ui.lock_navigation->isChecked());
		if (ui.expiration_checkbox->isChecked()) {
			m_root->setExpirationDate(ui.expiration_date->date());
		}
		else {
			m_root->setNoExpiration();
		}
		// don't reassign lock if nothing changed
	}

	if (m_unlock) {
		m_root->setSettingsLocked(false);
		m_root->setRestrictToCurrent(false);
		m_root->setNavigationLocked(false);

		m_root->setLockTable(m_this_lock);
	}
}

QStringList LockDialog::resultsString()
{

	QStringList msg;

	if (m_new_lock_pw) {
		msg.push_back("File was locked with a password.");
	}
	else if (m_new_lock_perm || m_change_pw_perm) {
		msg.push_back("File was permanently locked.");
	}
	else if (m_change_pw) {
		msg.push_back("Password was changed.");
	}
	else if (m_unlock) {
		msg.push_back("File was unlocked.");
	}

	if (m_nar_lock_ok > 0)
		msg.push_back(QString("Locked %1 narratives.").arg(m_nar_lock_ok));
	if (m_nar_lock_fail > 0)
		msg.push_back(QString("Failed to lock %1 narratives.").arg(m_nar_lock_fail));
	if (m_er_lock_ok > 0)
		msg.push_back(QString("Locked %1 resources.").arg(m_er_lock_ok));
	if (m_er_lock_fail > 0)
		msg.push_back(QString("Failed to lock %1 resources.").arg(m_er_lock_fail));
	if (m_models_embedded > 0)
		msg.push_back(QString("Embedded %1 models.").arg(m_models_embedded));

	return msg;
}

void LockDialog::execResultsMessage()
{
	QStringList results = resultsString();

	if (!results.isEmpty()) {
		QMessageBox::information(nullptr, m_title, results.join('\n'));
	}
}

QDialog::DialogCode LockDialog::execInternal(VSimRoot *root)
{
	m_root = root;

	// copy over the lock table
	m_this_lock = *root->lockTableConst();
	bool was_locked = m_this_lock.isLocked();
	m_was_locked = was_locked;
	bool has_pw = m_this_lock.hasPassword();
	m_lock_applied = false;

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

	ui.file_widget->setVisible(m_package);

	ui.change_password->setVisible(pw_locked);
	ui.change_password->setChecked(false);
	ui.use_password->setVisible(!hard_locked);
	ui.use_password->setChecked(true);
	ui.embed_models->setChecked(m_package);
	ui.embed_models->setEnabled(!root->settingsLocked());
	ui.lock_all->setVisible(!hard_locked);
	ui.meta_label->setVisible(was_locked);
	ui.lock_group_box->setEnabled(!was_locked);

	// init expiration date
	ui.expiration_checkbox->setChecked(root->expires());
	if (root->expires()) {
		ui.expiration_date->setDate(root->expirationDate());
	}
	else {
		QDate new_date = QDate::currentDate();
		new_date.addYears(10);
		ui.expiration_date->setDate(new_date);
	}

	checkEnablePassword();

	this->adjustSize();

	// exec() - see tryAccept();
	int result = exec();

	if (result == QDialog::Rejected) {
		return QDialog::Rejected;
	}

	m_lock = ui.lock_group_box->isChecked();
	m_new_lock_pw = m_lock && !m_was_locked && ui.use_password->isChecked();
	m_new_lock_perm = m_lock && !m_was_locked && !ui.use_password->isChecked();
	m_change_pw = m_lock && m_was_locked && ui.use_password->isChecked()
		&& ui.change_password->isChecked();
	m_change_pw_perm = m_lock && m_was_locked && !ui.use_password->isChecked();
	m_unlock = !m_lock && m_was_locked;
	m_pw = ui.password->text();

	return QDialog::Accepted;
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

bool LockDialog::onChooseFile()
{
	QString start;
	start = ui.file_text->text();
	QString filename = QFileDialog::getSaveFileName(this,
		m_title,
		start,
		"VSim files (*.vsim);;"
		"All types (*.*)");

	if (filename == "") {
		return false;
	}
	ui.file_text->setText(filename);
	return true;
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
