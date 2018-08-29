#include "AboutDialog.h"

AboutDialog::AboutDialog(QWidget * parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	setWindowTitle("About VSim");
}
