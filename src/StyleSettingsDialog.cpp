#include "StyleSettingsDialog.h"

#include <QColorDialog>
#include "StyleSettingsWidget.h"
#include "LabelStyleGroup.h"
#include "LabelStyle.h"
#include <QDebug>

StyleSettingsDialog::StyleSettingsDialog(QWidget *parent)
	: QDialog(parent),
	m_styles(nullptr)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(ui.confirm->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this,
		[this]() {
		takeStyles();
	});
	connect(ui.confirm->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this,
		[this]() {
		takeStyles();
	});
}

void StyleSettingsDialog::setStyles(LabelStyleGroup *styles)
{
	ui.tabs->clear();
	m_styles = styles;
	if (!styles) return;

	StyleSettingsWidget *tab;
	tab = new StyleSettingsWidget(ui.tabs);
	ui.tabs->addTab(tab, "Header 1");
	tab->setStyle(styles->getStyle(LabelType::HEADER1));
	m_header1_widget = tab;

	tab = new StyleSettingsWidget(ui.tabs);
	ui.tabs->addTab(tab, "Header 2");
	tab->setStyle(styles->getStyle(LabelType::HEADER2));
	m_header2_widget = tab;

	tab = new StyleSettingsWidget(ui.tabs);
	ui.tabs->addTab(tab, "Body");
	tab->setStyle(styles->getStyle(LabelType::BODY));
	m_body_widget = tab;

	tab = new StyleSettingsWidget(ui.tabs);
	ui.tabs->addTab(tab, "Label");
	tab->setStyle(styles->getStyle(LabelType::LABEL));
	m_label_widget = tab;
}

void StyleSettingsDialog::takeStyles(LabelStyleGroup *styles)
{
	if (!m_styles) return;
	if (!styles) styles = m_styles;

	LabelStyle *header1 = styles->getStyle(LabelType::HEADER1);
	header1->copy(m_header1_widget->getStyle());

	LabelStyle *header2 = styles->getStyle(LabelType::HEADER2);
	header2->copy(m_header2_widget->getStyle());

	LabelStyle *body = styles->getStyle(LabelType::BODY);
	body->copy(m_body_widget->getStyle());

	LabelStyle *label = styles->getStyle(LabelType::LABEL);
	label->copy(m_label_widget->getStyle());

	emit sApplied();
}
