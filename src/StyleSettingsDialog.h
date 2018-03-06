#ifndef STYLESETTINGSDIALOG_H
#define STYLESETTINGSDIALOG_H
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_StyleSettingsDialog.h"

class LabelStyleGroup;

class StyleSettingsDialog : public QDialog {
	Q_OBJECT
public:
	StyleSettingsDialog(QWidget *parent = nullptr);
	//StyleSettingsDialog(LabelStyleGroup *styles, QWidget *parent = nullptr);

	void setStyles(const LabelStyleGroup *styles);

private:
	QColor color;
	QColor bgColor;
	Ui::StyleSettingsDialog ui;
};

#endif