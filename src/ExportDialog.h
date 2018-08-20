#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QActionGroup>

#include "ui_ExportDialog.h"

class ExportDialog : public QDialog {
	Q_OBJECT;

public:
	ExportDialog(QWidget *parent = nullptr);

	virtual int exec() override;

	QString path() const;
	bool exportAll() const; // otherwise export selected
	bool lock() const;
	bool usePassword() const;
	QString password() const;

	void setTitle(QString title);
	void setTypes(QString types); // in qt file dialog format

private:
	bool onChooseFile();

private:
	Ui::ExportDialog ui;

	QString m_title;
	QString m_types;
};

#endif // ERDISPLAY_H