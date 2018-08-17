﻿#ifndef LOCKDIALOG_H
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

	void execEdit(VSimRoot *root);

	// configurations

	// 1. first time (add password)
	// 2. second time, edit button, has pw (change password button)
	// 3. package

	//QString password() const;
	//bool locked() const;
	//bool lockSettings() const;
	//bool lockNarrativesResources() const;
	//bool restrictToCurrent() const;
	//bool disableNavigation() const;
	//bool embedModels() const;
	//uint64_t expirationDateJulian() const;

	void execUnlockThis();

private:
	void tryAccept();

	void checkEnablePassword();

private:
	Ui::LockDialog ui;

	VSimRoot *m_root;
	//bool m_this_locked;
	QString m_this_password;
	//HashLock m_this_hash;
	LockTable m_this_lock;
	bool m_was_locked;
};

#endif // MAINWINDOWTOPBAR_H