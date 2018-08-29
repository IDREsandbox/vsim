#ifndef NAVIGATIONSETTINGSDIALOG_H
#define NAVIGATIONSETTINGSDIALOG_H

#include "ui_NavigationSettingsDialog.h"

class VSimApp;
class OSGViewerWidget;

class NavigationSettingsDialog : public QDialog {
	Q_OBJECT;
public:
	NavigationSettingsDialog(VSimApp *app, QWidget *parent = nullptr);

private:
	void load();
	void apply(VSimApp *app);
	void applyPreset(int i);
	void defaults();
	void updateTickMultipliers();

	void setReadOnly(bool read_only);

private:
	Ui::NavigationSettingsDialog ui;
	OSGViewerWidget *m_viewer;
	bool m_read_only;
};

#endif