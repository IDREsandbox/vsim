#ifndef DISPLAYSETTINGSDIALOG_H
#define DISPLAYSETTINGSDIALOG_H

#include "ui_DisplaySettingsDialog.h"

class VSimApp;
class OSGViewerWidget;

class DisplaySettingsDialog : public QDialog {
	Q_OBJECT;
public:
	DisplaySettingsDialog(VSimApp *app, QWidget *parent = nullptr);

private:
	void reload();
	void loadFov(float fov);
	void loadDefaults();

	void setFov(float fov);

public:
	Ui::DisplaySettingsDialog ui;

	VSimApp *m_app;
	OSGViewerWidget *m_viewer;
};

#endif