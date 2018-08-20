#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QPushButton>

#include "ui_PasswordDialog.h"

class LockTable;

class CreatePasswordDialog : public QDialog {
	Q_OBJECT;
public:
	CreatePasswordDialog(QWidget *parent = nullptr);

	//void setUseCheckbox(bool use_checkbox);
	//void setUsePassword(bool use);
	bool usePassword() const;
	QString password() const;

	void setLabel(QString text);

//public:
//	static QString getPasswordCheckable(bool *ok,
//		bool *no_pw);
//	static QString getPassword(bool *ok, QString title,
//		QString label);

protected:
	void showEvent(QShowEvent *e) override;

private:
	void onUseChange();

public:
	Ui::PasswordDialog ui;
	bool m_use;
};

// Can be used to unlock a single lock with setLock()
// or a vector of locks with setLocks()
class TestPasswordDialog : public QDialog {
	Q_OBJECT;
public:
	TestPasswordDialog(QWidget *parent = nullptr);
	void setLabel(QString msg);

	void showSkipButton(bool show);

	void setLock(LockTable *lock);
	void setLocks(std::vector<LockTable*> locks);

	void tryAccept();

	bool unlocked() const;
	bool skipped() const;

	enum Result {
		Unlocked,
		Skipped,
		Canceled
	};

public:
	Ui::PasswordDialog ui;
	LockTable *m_lock;
	std::vector<LockTable*> m_locks;
	QPushButton *m_skip_button;
	Result m_result;
};

#endif