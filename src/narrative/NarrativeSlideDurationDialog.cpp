#include "NarrativeSlideDurationDialog.h"
#include <QCheckBox>

NarrativeSlideDurationDialog::NarrativeSlideDurationDialog(QWidget * parent) 
	: QDialog(parent) 
{
	ui.setupUi(this);

	connect(ui.onclick_checkbox, &QCheckBox::stateChanged, this, &NarrativeSlideDurationDialog::enableDisableDuration);
}

void NarrativeSlideDurationDialog::setDuration(bool checked, float duration)
{
	ui.duration_spinbox->setValue(duration);
	ui.onclick_checkbox->setChecked(checked);
	enableDisableDuration(ui.onclick_checkbox->checkState());
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

void NarrativeSlideDurationDialog::enableDisableDuration(int checkbox_state)
{
	if (checkbox_state == Qt::Checked) {
		ui.duration_spinbox->setEnabled(false);
		ui.duration_label->setEnabled(false);
	}
	else {
		ui.duration_spinbox->setEnabled(true);
		ui.duration_label->setEnabled(true);
	}
}
