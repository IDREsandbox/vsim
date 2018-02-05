#include "StyleSettingsDialog.h"

#include <QColorDialog>
#include "StyleSettingsWidget.h"
#include "LabelStyleGroup.h"

StyleSettingsDialog::StyleSettingsDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void StyleSettingsDialog::setStyles(const LabelStyleGroup *styles)
{
	StyleSettingsWidget *tab;
	tab = new StyleSettingsWidget(ui.tabs);
	ui.tabs->addTab(tab, "Header 1");

	tab = new StyleSettingsWidget(ui.tabs);
	ui.tabs->addTab(tab, "Header 2");

	tab = new StyleSettingsWidget(ui.tabs);
	ui.tabs->addTab(tab, "Body");

	tab = new StyleSettingsWidget(ui.tabs);
	ui.tabs->addTab(tab, "Label");
}
