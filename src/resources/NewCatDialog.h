#ifndef NEWCATDOALOG_H
#define NEWCATDIALOG_H
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_NewCatDialog.h"
#include "resources/EResource.h"

class NewCatDialog : public QDialog {
	Q_OBJECT
public:
	NewCatDialog(QString window_title = "New Category", QWidget *parent = nullptr);
	~NewCatDialog();

	void setTitle(QString title);
	QString getCatTitle() const;
	void setColor(QColor);
	QColor getColor() const;

public slots:
	void pickColor();

private:
	Ui::NewCatDialog ui;
	QColor m_color;
};

#endif