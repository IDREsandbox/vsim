#include "HelpDialog.h"

#include <QDebug>

HelpDialog::HelpDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}
