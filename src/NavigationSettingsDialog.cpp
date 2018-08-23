#include "NavigationSettingsDialog.h"

#include <QDebug>

#include "VSimApp.h"
#include "VSimRoot.h"
#include "NavigationControl.h"
#include "OSGViewerWidget.h"

NavigationSettingsDialog::NavigationSettingsDialog(VSimApp *app, QWidget *parent)
	: QDialog(parent),
	m_read_only(false)
{
	ui.setupUi(this);

	connect(ui.tick_spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this,
		&NavigationSettingsDialog::updateTickMultipliers);
	connect(ui.tick_startup_spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this,
		&NavigationSettingsDialog::updateTickMultipliers);

	// tick limits
	int tick_limit = OSGViewerWidget::k_tick_limit;
	ui.tick_spinbox->setMinimum(-tick_limit);
	ui.tick_spinbox->setMaximum(tick_limit);
	ui.tick_startup_spinbox->setMinimum(-tick_limit);
	ui.tick_startup_spinbox->setMaximum(tick_limit);

	setReadOnly(app->getRoot()->settingsLocked());

	// presets
	ui.presets_combobox->addActions({});

	Preset meters, cm, feet;
	meters.name = "meters";
	meters.base_speed = 10.0;
	meters.speed_tick = 0;
	meters.collision_radius = .7;
	meters.eye_height = 1.7;
	meters.gravity_acceleration = 10.0;
	meters.gravity_fall_speed = 40.0;

	m_default = meters;

	auto scalePreset = [](const Preset &base, float scale, Preset *out) {
		out->base_speed = base.base_speed * scale;
		out->speed_tick = base.speed_tick;
		out->collision_radius = base.collision_radius * scale;
		out->eye_height = base.eye_height * scale;
		out->gravity_acceleration = base.gravity_acceleration * scale;
		out->gravity_fall_speed = base.gravity_fall_speed * scale;
	};

	cm.name = "cm";
	scalePreset(meters, 100.0, &cm);

	feet.name = "feet";
	scalePreset(meters, 3.0, &feet);

	m_presets.push_back(meters);
	m_presets.push_back(cm);
	m_presets.push_back(feet);

	// add items to combobox
	ui.presets_combobox->addItem("");
	for (auto &p : m_presets) {
		ui.presets_combobox->addItem(p.name);
	}

	connect(ui.presets_combobox,
		QOverload<int>::of(&QComboBox::activated), this, [this](int i) {
		ui.presets_combobox->setCurrentIndex(0);
		int pi = i - 1;
		if (pi < 0 || pi >= m_presets.size()) return;
		applyPreset(m_presets[pi]);
	});

	OSGViewerWidget *viewer = app->viewer();
	m_viewer = viewer;

	// home position
	connect(ui.home_set_button, &QAbstractButton::pressed, this, [this, viewer]() {
		viewer->setHomePosition(viewer->getCameraMatrix());
	});
	connect(ui.home_reset_button, &QAbstractButton::pressed, this, [this, viewer]() {
		viewer->resetHomePosition();
	});

	connect(ui.button_box, &QDialogButtonBox::clicked, this, [this](QAbstractButton *button) {
		if (ui.button_box->buttonRole(button) == QDialogButtonBox::ResetRole) {
			defaults();
		}
	});

	connect(this, &QDialog::accepted, this, [this, app]() {
		apply(app);
	});

	load();
}

void NavigationSettingsDialog::load()
{
	auto *viewer = m_viewer;

	// first person
	ui.speed_spinbox->setValue(viewer->baseSpeed());
	ui.tick_spinbox->setValue(viewer->speedTick());
	ui.tick_startup_spinbox->setValue(viewer->startupSpeedTick());
	updateTickMultipliers();

	// collisions
	ui.collision_checkbox->setChecked(viewer->collisionsEnabled());
	ui.collision_startup_checkbox->setChecked(viewer->collisionOnStartup());
	ui.collision_radius_spinbox->setValue(viewer->collisionRadius());

	// ground mode
	ui.ground_checkbox->setChecked(viewer->groundModeEnabled());
	ui.ground_startup_checkbox->setChecked(viewer->groundOnStartup());
	ui.height_spinbox->setValue(viewer->eyeHeight());
	ui.gravity_acceleration_spinbox->setValue(-viewer->gravityAcceleration()); // negate
	ui.gravity_speed_spinbox->setValue(viewer->gravitySpeed());

}

void NavigationSettingsDialog::apply(VSimApp * app)
{
	if (m_read_only) return;
	OSGViewerWidget *viewer = app->viewer();

	// first person
	viewer->setBaseSpeed(ui.speed_spinbox->value());
	viewer->setSpeedTick(ui.tick_spinbox->value());
	viewer->setStartupSpeedTick(ui.tick_startup_spinbox->value());

	// collisions
	viewer->enableCollisions(ui.collision_checkbox->isChecked());
	viewer->setCollisionOnStartup(ui.collision_startup_checkbox->isChecked());
	viewer->setCollisionRadius(ui.collision_radius_spinbox->value());

	// ground mode
	viewer->setGroundOnStartup(ui.ground_startup_checkbox->isChecked());
	viewer->enableGroundMode(ui.ground_checkbox->isChecked());
	viewer->setEyeHeight(ui.height_spinbox->value());
	viewer->setGravityAcceleration(-ui.gravity_acceleration_spinbox->value()); // negate
	viewer->setGravitySpeed(ui.gravity_speed_spinbox->value());
}

void NavigationSettingsDialog::defaults()
{
	applyPreset(m_default);

	ui.collision_checkbox->setChecked(true);
	ui.collision_startup_checkbox->setChecked(true);
	ui.ground_checkbox->setChecked(false);
	ui.ground_startup_checkbox->setChecked(false);
}

void NavigationSettingsDialog::applyPreset(const Preset & preset)
{
	// first person
	ui.speed_spinbox->setValue(preset.base_speed);
	ui.tick_spinbox->setValue(preset.speed_tick);
	ui.tick_startup_spinbox->setValue(preset.speed_tick);

	// collisions
	ui.collision_radius_spinbox->setValue(preset.collision_radius);

	// ground mode
	ui.height_spinbox->setValue(preset.eye_height);
	ui.gravity_acceleration_spinbox->setValue(preset.gravity_acceleration);
	ui.gravity_speed_spinbox->setValue(preset.gravity_fall_speed);
}

void NavigationSettingsDialog::updateTickMultipliers()
{
	QString text;
	text.sprintf("x%f", OSGViewerWidget::speedMultiplier(
		ui.tick_spinbox->value()));
	ui.tick_multiplier_label->setText(text);

	QString text2;
	text2.sprintf("x%f", OSGViewerWidget::speedMultiplier(
		ui.tick_startup_spinbox->value()));
	ui.tick_startup_multiplier_label->setText(text);
}

void NavigationSettingsDialog::setReadOnly(bool read_only)
{
	m_read_only = read_only;
	bool enable = !read_only;

	ui.home_set_button->setEnabled(enable);
	ui.home_reset_button->setEnabled(enable);
	ui.presets_combobox->setEnabled(enable);
	ui.speed_spinbox->setEnabled(enable);
	ui.tick_spinbox->setEnabled(enable);
	ui.tick_startup_spinbox->setEnabled(enable);
	ui.collision_checkbox->setEnabled(enable);
	ui.collision_startup_checkbox->setEnabled(enable);
	ui.collision_radius_spinbox->setEnabled(enable);
	ui.ground_checkbox->setEnabled(enable);
	ui.ground_startup_checkbox->setEnabled(enable);
	ui.height_spinbox->setEnabled(enable);
	ui.gravity_acceleration_spinbox->setEnabled(enable);
	ui.gravity_speed_spinbox->setEnabled(enable);

	ui.button_box->button(QDialogButtonBox::RestoreDefaults)->setVisible(enable);
	ui.button_box->button(QDialogButtonBox::Cancel)->setVisible(enable);
}
