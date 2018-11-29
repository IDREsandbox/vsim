#include "OtherSettingsDialog.h"

#include <QDebug>

#include "VSimApp.h"
#include "VSimRoot.h"
#include "narrative/NarrativePlayer.h"
#include "Core/LockTable.h"

OtherSettingsDialog::OtherSettingsDialog(QWidget *parent)
	: QDialog(parent),
	m_read_only(false)
{
	ui.setupUi(this);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	// TODO
	ui.year_enabled->hide();
	ui.year_label->hide();
	ui.year_spinbox->hide();
	ui.year_use_slider->hide();
}

int OtherSettingsDialog::exec2(VSimApp * app)
{
	OtherSettingsDialog dlg;
	dlg.load(app);
	int result = dlg.exec();
	if (result == QDialog::Accepted) {
		dlg.apply(app);
	}
	return result;
}

void OtherSettingsDialog::load(VSimApp *app)
{
	bool locked = app->getRoot()->settingsLocked();
	m_read_only = locked;
	ui.narrative_cycling_checkbox->setEnabled(!locked);

	NarrativePlayer *nc = app->narrativePlayer();
	ui.narrative_cycling_checkbox->setChecked(nc->narrativeCycling());

	//TimeManager *tm = app->timeManager();
}

void OtherSettingsDialog::apply(VSimApp *app)
{
	if (m_read_only) return;

	NarrativePlayer *nc = app->narrativePlayer();
	nc->setNarrativeCycling(ui.narrative_cycling_checkbox->isChecked());
}
