#ifndef STYLESETTINGSDIALOG_H
#define STYLESETTINGSDIALOG_H
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_StyleSettingsDialog.h"

class LabelStyleGroup;
class StyleSettingsWidget;

class StyleSettingsDialog : public QDialog {
	Q_OBJECT
public:
	StyleSettingsDialog(QWidget *parent = nullptr);

	void setStyles(LabelStyleGroup *styles); // group -> widget, applies styles to StyleSettingWidgets

	// widget -> group, takes styles from this dialog and applies them to group
	// uses the last set m_styles if null
	void takeStyles(LabelStyleGroup *group = nullptr);

	// use sApplied for ok and accept signal,
	// the dialog already calls takeStyles
	// int exec();

signals:
	void sApplied();

private:
	Ui::StyleSettingsDialog ui;

	StyleSettingsWidget *m_header1_widget;
	StyleSettingsWidget *m_header2_widget;
	StyleSettingsWidget *m_body_widget;
	StyleSettingsWidget *m_label_widget;
	
	LabelStyleGroup *m_styles;
};

#endif