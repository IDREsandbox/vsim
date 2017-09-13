#include "StyleSettings.h"
#include <qcolordialog.h>
#include "LabelStyle.h"

StyleSettings::StyleSettings(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";
	ui.h1_sizeBox->addItems(fontSizes);
	ui.h2_sizeBox->addItems(fontSizes);
	ui.bt_sizeBox->addItems(fontSizes);
	ui.lt_sizeBox->addItems(fontSizes);
	ui.il_sizeBox->addItems(fontSizes);

	connect(ui.h1_colorPicker, &QPushButton::clicked, this, &StyleSettings::pickFontColor_h1);
	connect(ui.h1_colorPicker_bod, &QPushButton::clicked, this, &StyleSettings::pickBGColor_h1);
	connect(ui.h1_fontBox, &QFontComboBox::currentFontChanged, this, &StyleSettings::pickFont_h1);
	connect(ui.h1_sizeBox, &QComboBox::currentTextChanged, this, &StyleSettings::pickFontSize_h1);
	connect(ui.h1_opacity, &QSlider::valueChanged, this, &StyleSettings::pickFontOpacity_h1);
	connect(ui.h1_opacity_bod, &QSlider::valueChanged, this, &StyleSettings::pickBGOpacity_h1);

	ui.header1_prev->setAutoFillBackground(true);
}

StyleSettings::StyleSettings(Narrative2* narr, QWidget *parent)
	: StyleSettings(parent)
{
	n = narr;
}

StyleSettings::~StyleSettings()
{
}

void StyleSettings::pickFontOpacity_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setOpacity(ui.h1_opacity->value());

	QPalette pal = ui.header1_prev->palette();
	pal.setColor(ui.header1_prev->foregroundRole(), qRgba(color.red(), color.green(), color.blue(), ui.h1_opacity->value()));
	ui.header1_prev->setPalette(pal);

	n->setH1(lb);
}

void StyleSettings::pickBGOpacity_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setOpacity_BG(ui.h1_opacity_bod->value());

	QPalette pal = ui.header1_prev->palette();
	pal.setColor(ui.header1_prev->backgroundRole(), qRgba(bgColor.red(), bgColor.green(), bgColor.blue(), ui.h1_opacity_bod->value()));
	ui.header1_prev->setPalette(pal);

	n->setH1(lb);
}

void StyleSettings::pickFontSize_h1()
{
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";

	LabelStyle* lb = n->getH1();
	lb->setSize(fontSizes.at(ui.h1_sizeBox->currentIndex()).toInt());
	const QFont temp = ui.header1_prev->font();
	QFont temp2 = temp;
	temp2.setPointSize(fontSizes.at(ui.h1_sizeBox->currentIndex()).toInt());
	ui.header1_prev->setFont(temp2);
	//ui.header1_prev->setStyleSheet(QString::fromStdString("font-size:" + std::to_string(fontSizes.at(ui.h1_sizeBox->currentIndex()).toInt()) + "pt;"));
	n->setH1(lb);
}

void StyleSettings::pickFontColor_h1()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.h1_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));
	//ui.header1_prev->setStyleSheet(QString::fromStdString("color:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.header1_prev->palette();
	pal.setColor(ui.header1_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	ui.header1_prev->setPalette(pal);

	LabelStyle* lb = n->getH1();
	lb->setRed(color.red());
	lb->setBlue(color.blue());
	lb->setGreen(color.green());

	n->setH1(lb);
}

void StyleSettings::pickBGColor_h1()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	bgColor = dlg.selectedColor();
	ui.h1_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(bgColor.red()) + "," + std::to_string(bgColor.green()) + "," + std::to_string(bgColor.blue()) + ");"));
	//ui.header1_prev->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.header1_prev->palette();
	pal.setColor(ui.header1_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	ui.header1_prev->setPalette(pal);

	LabelStyle* lb = n->getH1();
	lb->setRed_BG(bgColor.red());
	lb->setBlue_BG(bgColor.blue());
	lb->setGreen_BG(bgColor.green());

	n->setH1(lb);
}

void StyleSettings::pickFont_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setFont(ui.h1_fontBox->currentFont().family().toStdString());
	const QFont temp = ui.header1_prev->font();
	QFont temp2 = temp;
	temp2.setFamily(ui.h1_fontBox->currentFont().family());
	ui.header1_prev->setFont(temp2);
	//ui.header1_prev->setStyleSheet(QString::fromStdString("font-family:" + ui.h1_fontBox->currentFont().family().toStdString() + ";"));
	n->setH1(lb);
}