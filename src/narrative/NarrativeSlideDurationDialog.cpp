#include "NarrativeSlideDurationDialog.h"
#include <QCheckBox>
#include <QDebug>

bool NarrativeSlideDurationDialog::create(bool init_stay, float init_duration, bool *out_stay, float *out_duration) {
	NarrativeSlideDurationDialog dialog;
	dialog.setWindowFlags(Qt::WindowSystemMenuHint);
	dialog.setDuration(init_stay, init_duration);
	int result = dialog.exec();
	if (result == QDialog::Rejected) {
		return false;
	}
	*out_stay = dialog.getStayOnNode();
	*out_duration = dialog.getDuration();
	return true;
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

float NarrativeSlideDurationDialog::getDuration() const
{
	return ui.duration_spinbox->value();
}

bool NarrativeSlideDurationDialog::getStayOnNode() const
{
	return ui.onclick_checkbox->isChecked();
}

void NarrativeSlideDurationDialog::enableDuration(bool enable)
{
	ui.duration_spinbox->setEnabled(enable);
	ui.duration_label->setEnabled(enable);
}
