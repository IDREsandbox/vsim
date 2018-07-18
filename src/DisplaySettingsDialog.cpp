#include "DisplaySettingsDialog.h"

#include "VSimApp.h"
#include "NavigationControl.h"
#include "OSGViewerWidget.h"

DisplaySettingsDialog::DisplaySettingsDialog(VSimApp *app, QWidget *parent)
	: QDialog(parent),
	m_app(app)
{
	ui.setupUi(this);

	m_viewer = app->viewer();

	// camera settings
	// live connection

	// fov - connect spinbox to slider
	connect(ui.fov_slider, &QAbstractSlider::valueChanged, this,
		&DisplaySettingsDialog::setFov);
	connect(ui.fov_spinbox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
		&DisplaySettingsDialog::setFov);

	connect(ui.clip_auto_checkbox, &QAbstractButton::clicked, this, [this](bool checked) {
		m_viewer->setAutoClip(checked);
		reload();
	});

	connect(ui.clip_near_spinbox, &QAbstractSpinBox::editingFinished, this, [this]() {
		m_viewer->setNearClip(ui.clip_near_spinbox->value());
		reload();
	});
	connect(ui.clip_far_spinbox, &QAbstractSpinBox::editingFinished, this, [this]() {
		m_viewer->setFarClip(ui.clip_far_spinbox->value());
		reload();
	});

	connect(ui.button_box, &QDialogButtonBox::clicked, this, [this](QAbstractButton *button) {
		if (ui.button_box->buttonRole(button) == QDialogButtonBox::ResetRole) {
			loadDefaults();
		}
	});

	reload();
}

void DisplaySettingsDialog::reload()
{
	bool auto_clip = m_viewer->autoClip();
	ui.clip_auto_checkbox->setChecked(auto_clip);

	ui.clip_near_spinbox->setEnabled(!auto_clip);
	ui.clip_far_spinbox->setEnabled(!auto_clip);
	ui.clip_near_spinbox->setValue(m_viewer->nearClip());
	ui.clip_far_spinbox->setValue(m_viewer->farClip());


	loadFov(m_viewer->fovy());
}

void DisplaySettingsDialog::loadFov(float fov)
{
	ui.fov_slider->blockSignals(true);
	ui.fov_slider->setValue(std::round(fov));
	ui.fov_slider->blockSignals(false);

	ui.fov_spinbox->blockSignals(true);
	ui.fov_spinbox->setValue(fov);
	ui.fov_spinbox->blockSignals(false);
}

void DisplaySettingsDialog::loadDefaults()
{
	m_viewer->setFovy(55);
	m_viewer->setAutoClip(true);
	m_viewer->setNearClip(.5);
	m_viewer->setFarClip(5000);
	reload();
}

void DisplaySettingsDialog::setFov(float fov)
{
	m_app->viewer()->setFovy(fov);
	reload();
}
