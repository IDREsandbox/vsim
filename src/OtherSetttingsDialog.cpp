#include "OtherSettingsDialog.h"

#include <QDebug>

#include "VSimApp.h"
#include "NavigationControl.h"
#include "TimeManager.h"

OtherSettingsDialog::OtherSettingsDialog(VSimApp *app, QWidget *parent)
	: QDialog(parent),
	m_read_only(false)
{
	ui.setupUi(this);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

void OtherSettingsDialog::load(VSimApp *app)
{
}

void OtherSettingsDialog::apply(VSimApp *app)
{
	if (m_read_only) return;
}
