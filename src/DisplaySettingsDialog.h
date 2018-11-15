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
	void loadAmbient(QColor c);

	void setFov(float fov);

	void setReadOnly(bool read_only);

public:
	Ui::DisplaySettingsDialog ui;

	VSimApp *m_app;
	OSGViewerWidget *m_viewer;
	QPushButton *m_defaults_button;
	bool m_read_only;
};

#endif