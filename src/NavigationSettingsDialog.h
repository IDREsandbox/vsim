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
	void defaults();
	void updateTickMultipliers();

	void setReadOnly(bool read_only);
private:
	struct Preset {
		const char *name;
		float base_speed;
		int speed_tick;
		float collision_radius;
		float eye_height;
		float gravity_acceleration; // positive
		float gravity_fall_speed;
	};

	void applyPreset(const Preset &preset);

private:
	Ui::NavigationSettingsDialog ui;
	std::vector<Preset> m_presets;
	Preset m_default;
	OSGViewerWidget *m_viewer;
	bool m_read_only;
};

#endif