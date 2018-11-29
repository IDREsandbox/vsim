#ifndef OTHERSETTINGSDIALOG_H
#define OTHERSETTINGSDIALOG_H

#include "ui_OtherSettingsDialog.h"

#include <QDialog>

class VSimApp;

class OtherSettingsDialog : public QDialog {
	Q_OBJECT;
public:
	OtherSettingsDialog(QWidget *parent = nullptr);

	// self-applying
	static int exec2(VSimApp *app);

private:
	void load(VSimApp *app);
	void apply(VSimApp *app);
	void setReadOnly(bool read_only);

private:
	Ui::OtherSettingsDialog ui;
	bool m_read_only;
};

#endif