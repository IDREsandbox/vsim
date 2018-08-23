#ifndef LOCKDIALOG_H
#define LOCKDIALOG_H

#include <QDialog>

#include "ui_LockDialog.h"
#include "Core/HashLock.h"
#include "Core/LockTable.h"

class VSimRoot;

class LockDialog : public QDialog {
	Q_OBJECT;

public:
	LockDialog(QWidget * parent = nullptr);

	// configurations

	// 1. first time (add password)
	// 2. second time, edit button, has pw (change password button)
	// 3. packaging

	// for lock settings
	// performs locking operations on root
	void execEdit(VSimRoot *root);

	// for packaging
	// performs locking operations on root
	// also picks a file, get with filePath()
	QDialog::DialogCode execPackage(VSimRoot *root);

	//QString password() const;
	//bool locked() const;
	//bool lockSettings() const;
	//bool lockNarrativesResources() const;
	//bool restrictToCurrent() const;
	//bool disableNavigation() const;
	//bool embedModels() const;
	//uint64_t expirationDateJulian() const;

	void execUnlockThis();

	// apply changes to root
	void apply();

	// queries
	// retuns a list of various results
	// # nars locked/failed, # ers locked/failed, models embedded, etc
	QStringList resultsString();
	void execResultsMessage();
	QString filePath() const;

	// did we lock something? (needed for clearing the undo stack)
	bool lockApplied() const;

private:
	QDialog::DialogCode execInternal(VSimRoot *root);
	void tryAccept();

	bool onChooseFile();

	void checkEnablePassword();

private:
	Ui::LockDialog ui;

	// configuration
	bool m_package;
	QString m_title;

	// before
	VSimRoot *m_root;
	//bool m_this_locked;
	QString m_this_password;
	//HashLock m_this_hash;
	LockTable m_this_lock;
	bool m_was_locked;

	// results
	bool m_lock;
	bool m_new_lock_pw;
	bool m_new_lock_perm;
	bool m_change_pw;
	bool m_change_pw_perm;
	bool m_unlock;
	QString m_pw;

	// apply results
	int m_nar_lock_ok;
	int m_nar_lock_fail;
	int m_er_lock_ok;
	int m_er_lock_fail;
	int m_models_embedded;
	bool m_lock_applied;
};

#endif // MAINWINDOWTOPBAR_H