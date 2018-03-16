#include "NarrativeSlideDurationDialog.h"
#include <QCheckBox>
#include <QDebug>

float NarrativeSlideDurationDialog::create(bool init_stay, float init_duration) {
	NarrativeSlideDurationDialog *dialog = new NarrativeSlideDurationDialog(nullptr);
	dialog->setWindowFlags(Qt::WindowSystemMenuHint);
	dialog->setDuration(init_stay, init_duration);
	int result = dialog->exec();
	float duration;
	if (result == QDialog::Rejected) {
		duration = -1.0f;
	}
	else {
		duration = dialog->getDuration();
	}
	delete dialog;
	return duration;
}

NarrativeSlideDurationDialog::NarrativeSlideDurationDialog(QWidget * parent) 
	: QDialog(parent) 
{
	ui.setupUi(this);

	connect(ui.onclick_checkbox, &QAbstractButton::toggled, this,
		[this] (bool stay) {
		enableDuration(!stay);
	});
}

void NarrativeSlideDurationDialog::setDuration(bool stay, float duration)
{
	ui.duration_spinbox->setValue(duration);
	ui.onclick_checkbox->setChecked(stay);
	enableDuration(!stay);
	if (!stay) {
		ui.duration_spinbox->setFocus();
	}
}

float NarrativeSlideDurationDialog::getDuration()
{
	if (ui.onclick_checkbox->isChecked()) {
		return 0.0f;
	}
	else {
		return ui.duration_spinbox->value();
	}
}

void NarrativeSlideDurationDialog::enableDuration(bool enable)
{
	ui.duration_spinbox->setEnabled(enable);
	ui.duration_label->setEnabled(enable);
}
