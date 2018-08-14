#ifndef LOCKDIALOG_H
#define LOCKDIALOG_H

#include <QDialog>

#include "ui_LockDialog.h"

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
	bool locked() const;
	bool lockSettings() const;
	bool lockNarrativesResources() const;
	bool restrictToCurrent() const;
	bool disableNavigation() const;
	bool embedModels() const;
	uint64_t expirationDateJulian() const;

private:
	void tryAccept();

	void checkEnablePassword();
	void checkEnableExpiration();

private:
	Ui::LockDialog ui;

	VSimRoot *m_root;
	bool m_this_locked;
};

#endif // MAINWINDOWTOPBAR_H