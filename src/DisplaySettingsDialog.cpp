#include "DisplaySettingsDialog.h"

#include <QPushButton>
#include <QDebug>

#include "VSimApp.h"
#include "OSGViewerWidget.h"
#include "VSimRoot.h"

// one way binding + modal
// There is no listening to the data, so if someone else changes these values
// then the dialog goes invalid. However, this should be a modal dialog so we
// should have full control.
DisplaySettingsDialog::DisplaySettingsDialog(VSimApp *app, QWidget *parent)
	: QDialog(parent),
	m_app(app),
	m_read_only(false)
{
	ui.setupUi(this);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	m_viewer = app->viewer();

	ui.fov_slider->setRange(1, 179);
	ui.fov_spinbox->setRange(.1, 179.9);
	ui.clip_near_spinbox->setRange(.001, 100000);
	ui.clip_far_spinbox->setRange(.001, 1000000);

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

	m_defaults_button = ui.button_box->button(QDialogButtonBox::RestoreDefaults);
	connect(ui.button_box, &QDialogButtonBox::clicked, this, [this](QAbstractButton *button) {
		if (button == m_defaults_button) {
			loadDefaults();
		}
	});

	setReadOnly(m_app->getRoot()->settingsLocked());

	reload();
	adjustSize();
}

void DisplaySettingsDialog::reload()
{
	bool auto_clip = m_viewer->autoClip();
	ui.clip_auto_checkbox->setChecked(auto_clip);

	ui.clip_near_spinbox->setValue(m_viewer->nearClip());
	ui.clip_far_spinbox->setValue(m_viewer->farClip());

	// read only
	bool enabled = !m_read_only;
	ui.fov_slider->setEnabled(enabled);
	ui.fov_spinbox->setEnabled(enabled);
	ui.clip_near_spinbox->setEnabled(!auto_clip && !m_read_only);
	ui.clip_far_spinbox->setEnabled(!auto_clip && !m_read_only);
	ui.clip_auto_checkbox->setEnabled(enabled);
	m_defaults_button->setVisible(enabled);

	loadFov(m_viewer->fovy());
}

void DisplaySettingsDialog::loadFov(float fov)
{
	ui.fov_slider->blockSignals(true);
	ui.fov_spinbox->blockSignals(true);
	ui.fov_slider->setValue(std::round(fov));
	ui.fov_spinbox->setValue(fov);
	ui.fov_spinbox->blockSignals(false);
	ui.fov_slider->blockSignals(false);
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

void DisplaySettingsDialog::setReadOnly(bool read_only)
{
	m_read_only = read_only;
	reload();
}
