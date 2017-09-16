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

	QStringList justifications;
	justifications << "Left" << "Center" << "Right" << "Justify";
	ui.h1_justify->addItems(justifications);

	QStringList styles;
	styles << "Regular" << "Bold" << "Italic";
	ui.h1_styleBox->addItems(styles);

	connect(ui.h1_colorPicker, &QPushButton::clicked, this, &StyleSettings::pickFontColor_h1);
	connect(ui.h1_colorPicker_bod, &QPushButton::clicked, this, &StyleSettings::pickBGColor_h1);
	connect(ui.h1_fontBox, &QFontComboBox::currentFontChanged, this, &StyleSettings::pickFont_h1);
	connect(ui.h1_sizeBox, &QComboBox::currentTextChanged, this, &StyleSettings::pickFontSize_h1);
	connect(ui.h1_opacity, &QSlider::valueChanged, this, &StyleSettings::pickFontOpacity_h1);
	connect(ui.h1_opacity_bod, &QSlider::valueChanged, this, &StyleSettings::pickBGOpacity_h1);
	connect(ui.h1_opacityBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &StyleSettings::pickFontOpacityBox_h1);
	connect(ui.h1_opacityBox_bod, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &StyleSettings::pickBGOpacityBox_h1);
	connect(ui.h1_width, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &StyleSettings::pickWidth_h1);
	connect(ui.h1_height, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &StyleSettings::pickHeight_h1);
	connect(ui.h1_styleBox, &QComboBox::currentTextChanged, this, &StyleSettings::pickStyle_h1);
	connect(ui.h1_justify, &QComboBox::currentTextChanged, this, &StyleSettings::pickJustification_h1);
	connect(ui.h1_margin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &StyleSettings::pickMargin_h1);

	connect(ui.h2_colorPicker, &QPushButton::clicked, this, &StyleSettings::pickFontColor_h2);
	connect(ui.h2_colorPicker_bod, &QPushButton::clicked, this, &StyleSettings::pickBGColor_h2);
	connect(ui.h2_fontBox, &QFontComboBox::currentFontChanged, this, &StyleSettings::pickFont_h2);
	connect(ui.h2_sizeBox, &QComboBox::currentTextChanged, this, &StyleSettings::pickFontSize_h2);
	connect(ui.h2_opacity, &QSlider::valueChanged, this, &StyleSettings::pickFontOpacity_h2);
	connect(ui.h2_opacity_bod, &QSlider::valueChanged, this, &StyleSettings::pickBGOpacity_h2);

	connect(ui.bt_colorPicker, &QPushButton::clicked, this, &StyleSettings::pickFontColor_bt);
	connect(ui.bt_colorPicker_bod, &QPushButton::clicked, this, &StyleSettings::pickBGColor_bt);
	connect(ui.bt_fontBox, &QFontComboBox::currentFontChanged, this, &StyleSettings::pickFont_bt);
	connect(ui.bt_sizeBox, &QComboBox::currentTextChanged, this, &StyleSettings::pickFontSize_bt);
	connect(ui.bt_opacity, &QSlider::valueChanged, this, &StyleSettings::pickFontOpacity_bt);
	connect(ui.bt_opacity_bod, &QSlider::valueChanged, this, &StyleSettings::pickBGOpacity_bt);

	connect(ui.lt_colorPicker, &QPushButton::clicked, this, &StyleSettings::pickFontColor_lt);
	connect(ui.lt_colorPicker_bod, &QPushButton::clicked, this, &StyleSettings::pickBGColor_lt);
	connect(ui.lt_fontBox, &QFontComboBox::currentFontChanged, this, &StyleSettings::pickFont_lt);
	connect(ui.lt_sizeBox, &QComboBox::currentTextChanged, this, &StyleSettings::pickFontSize_lt);
	connect(ui.lt_opacity, &QSlider::valueChanged, this, &StyleSettings::pickFontOpacity_lt);
	connect(ui.lt_opacity_bod, &QSlider::valueChanged, this, &StyleSettings::pickBGOpacity_lt);

	connect(ui.il_colorPicker, &QPushButton::clicked, this, &StyleSettings::pickFontColor_il);
	connect(ui.il_colorPicker_bod, &QPushButton::clicked, this, &StyleSettings::pickBGColor_il);
	connect(ui.il_fontBox, &QFontComboBox::currentFontChanged, this, &StyleSettings::pickFont_il);
	connect(ui.il_sizeBox, &QComboBox::currentTextChanged, this, &StyleSettings::pickFontSize_il);
	connect(ui.il_opacity, &QSlider::valueChanged, this, &StyleSettings::pickFontOpacity_il);
	connect(ui.il_opacity_bod, &QSlider::valueChanged, this, &StyleSettings::pickBGOpacity_il);

	ui.header1_prev->setAutoFillBackground(true);
	ui.header2_prev->setAutoFillBackground(true);
	ui.bt_prev->setAutoFillBackground(true);
	ui.lt_prev->setAutoFillBackground(true);
	ui.il_prev->setAutoFillBackground(true);

	ui.h1_width->setMaximum(1000);
	ui.h1_height->setMaximum(1000);
}

StyleSettings::StyleSettings(Narrative2* narr, QWidget *parent)
	: StyleSettings(parent)
{
	n = narr;

	//header 1
	LabelStyle* lb = n->getH1();
	ui.h1_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + ");"));
	ui.h1_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + ");"));
	ui.h1_fontBox->setFont(QFont(QString::fromStdString(lb->getFont())));
	
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";
	ui.h1_sizeBox->setCurrentIndex(fontSizes.lastIndexOf(QString::number(lb->getSize())));
	ui.h1_opacity->setValue(lb->getOpacity());
	ui.h1_opacity_bod->setValue(lb->getOpacity_BG());

	ui.h1_opacityBox->setValue(lb->getOpacity());
	ui.h1_opacityBox_bod->setValue(lb->getOpacity_BG());

	ui.h1_fontBox->setFont(QFont(QString::fromStdString(lb->getFont())));
	ui.header1_prev->setFont(QFont(QString::fromStdString(lb->getFont()), lb->getSize()));

	QStringList styles;
	styles << "Regular" << "Bold" << "Italic";
	ui.h1_styleBox->setCurrentIndex(styles.lastIndexOf(QString::fromStdString(lb->getWeight())));

	const QFont temp = ui.header1_prev->font();
	QFont temp2 = temp;
	if (lb->getWeight() == "Italic")
	{
		temp2.setItalic(true);
		temp2.setBold(false);
	}
	else if (lb->getWeight() == "Bold")
	{
		temp2.setBold(true);
		temp2.setItalic(false);
	}
	else
	{
		temp2.setBold(false);
		temp2.setItalic(false);
	}

	ui.header1_prev->setFont(temp2);

	QStringList justifications;
	justifications << "Left" << "Center" << "Right" << "Justify";
	ui.h1_justify->setCurrentIndex(justifications.lastIndexOf(QString::fromStdString(lb->getAlign())));

	if (lb->getAlign() == "Left")
		ui.header1_prev->setAlignment(Qt::AlignLeft);
	else if (lb->getAlign() == "Right")
		ui.header1_prev->setAlignment(Qt::AlignRight);
	else if (lb->getAlign() == "Justify")
		ui.header1_prev->setAlignment(Qt::AlignJustify);
	else
		ui.header1_prev->setAlignment(Qt::AlignHCenter);

	ui.header1_prev->setMargin(lb->getMargin());

	QPalette pal = ui.header1_prev->palette();
	pal.setColor(ui.header1_prev->foregroundRole(), qRgb(lb->getRed(), lb->getGreen(), lb->getBlue()));
	QColor color = pal.color(ui.header1_prev->foregroundRole());
	color.setAlpha(lb->getOpacity());
	pal.setColor(ui.header1_prev->foregroundRole(), color);

	pal.setColor(ui.header1_prev->backgroundRole(), qRgb(lb->getRed_BG(), lb->getGreen_BG(), lb->getBlue_BG()));
	color = pal.color(ui.header1_prev->backgroundRole());
	color.setAlpha(lb->getOpacity_BG());
	pal.setColor(ui.header1_prev->backgroundRole(), color);
	ui.header1_prev->setPalette(pal);

	ui.h1_width->setValue(lb->getWidth());
	ui.h1_height->setValue(lb->getHeight());

	ui.header1_prev->setFixedHeight(lb->getHeight());
	ui.header1_prev->setFixedWidth(lb->getWidth());

	//header 2
	lb = n->getH2();
	ui.h2_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + ");"));
	ui.h2_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + ");"));
	ui.h2_fontBox->setFont(QFont(QString::fromStdString(lb->getFont())));

	ui.h2_sizeBox->setCurrentIndex(fontSizes.lastIndexOf(QString::number(lb->getSize())));
	ui.h2_opacity->setValue(lb->getOpacity());
	ui.h2_opacity_bod->setValue(lb->getOpacity_BG());

	ui.h2_fontBox->setFont(QFont(QString::fromStdString(lb->getFont())));
	ui.header2_prev->setFont(QFont(QString::fromStdString(lb->getFont()), lb->getSize()));

	pal = ui.header2_prev->palette();
	pal.setColor(ui.header2_prev->foregroundRole(), qRgb(lb->getRed(), lb->getGreen(), lb->getBlue()));
	color = pal.color(ui.header2_prev->foregroundRole());
	color.setAlpha(lb->getOpacity());
	pal.setColor(ui.header2_prev->foregroundRole(), color);

	pal.setColor(ui.header2_prev->backgroundRole(), qRgb(lb->getRed_BG(), lb->getGreen_BG(), lb->getBlue_BG()));
	color = pal.color(ui.header2_prev->backgroundRole());
	color.setAlpha(lb->getOpacity_BG());
	pal.setColor(ui.header2_prev->backgroundRole(), color);
	ui.header2_prev->setPalette(pal);

	ui.h2_width->setValue(lb->getWidth());
	ui.h2_height->setValue(lb->getHeight());

	ui.header2_prev->setFixedHeight(lb->getHeight());
	ui.header2_prev->setFixedWidth(lb->getWidth());

	//body text
	lb = n->getBod();
	ui.bt_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + ");"));
	ui.bt_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + ");"));
	ui.bt_fontBox->setFont(QFont(QString::fromStdString(lb->getFont())));

	ui.bt_sizeBox->setCurrentIndex(fontSizes.lastIndexOf(QString::number(lb->getSize())));
	ui.bt_opacity->setValue(lb->getOpacity());
	ui.bt_opacity_bod->setValue(lb->getOpacity_BG());

	ui.bt_fontBox->setFont(QFont(QString::fromStdString(lb->getFont())));
	ui.bt_prev->setFont(QFont(QString::fromStdString(lb->getFont()), lb->getSize()));

	pal = ui.bt_prev->palette();
	pal.setColor(ui.bt_prev->foregroundRole(), qRgb(lb->getRed(), lb->getGreen(), lb->getBlue()));
	color = pal.color(ui.bt_prev->foregroundRole());
	color.setAlpha(lb->getOpacity());
	pal.setColor(ui.bt_prev->foregroundRole(), color);

	pal.setColor(ui.bt_prev->backgroundRole(), qRgb(lb->getRed_BG(), lb->getGreen_BG(), lb->getBlue_BG()));
	color = pal.color(ui.bt_prev->backgroundRole());
	color.setAlpha(lb->getOpacity_BG());
	pal.setColor(ui.bt_prev->backgroundRole(), color);
	ui.bt_prev->setPalette(pal);

	ui.bt_width->setValue(lb->getWidth());
	ui.bt_height->setValue(lb->getHeight());

	ui.bt_prev->setFixedHeight(lb->getHeight());
	ui.bt_prev->setFixedWidth(lb->getWidth());

	//label text
	lb = n->getLab();
	ui.lt_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + ");"));
	ui.lt_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + ");"));
	ui.lt_fontBox->setFont(QFont(QString::fromStdString(lb->getFont())));

	ui.lt_sizeBox->setCurrentIndex(fontSizes.lastIndexOf(QString::number(lb->getSize())));
	ui.lt_opacity->setValue(lb->getOpacity());
	ui.lt_opacity_bod->setValue(lb->getOpacity_BG());

	ui.lt_fontBox->setFont(QFont(QString::fromStdString(lb->getFont())));
	ui.lt_prev->setFont(QFont(QString::fromStdString(lb->getFont()), lb->getSize()));

	pal = ui.lt_prev->palette();
	pal.setColor(ui.lt_prev->foregroundRole(), qRgb(lb->getRed(), lb->getGreen(), lb->getBlue()));
	color = pal.color(ui.lt_prev->foregroundRole());
	color.setAlpha(lb->getOpacity());
	pal.setColor(ui.lt_prev->foregroundRole(), color);

	pal.setColor(ui.lt_prev->backgroundRole(), qRgb(lb->getRed_BG(), lb->getGreen_BG(), lb->getBlue_BG()));
	color = pal.color(ui.lt_prev->backgroundRole());
	color.setAlpha(lb->getOpacity_BG());
	pal.setColor(ui.lt_prev->backgroundRole(), color);
	ui.lt_prev->setPalette(pal);

	ui.lt_width->setValue(lb->getWidth());
	ui.lt_height->setValue(lb->getHeight());

	ui.lt_prev->setFixedHeight(lb->getHeight());
	ui.lt_prev->setFixedWidth(lb->getWidth());

	//image label
	lb = n->getImg();
	ui.il_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + ");"));
	ui.il_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + ");"));
	ui.il_fontBox->setFont(QFont(QString::fromStdString(lb->getFont())));

	ui.il_sizeBox->setCurrentIndex(fontSizes.lastIndexOf(QString::number(lb->getSize())));
	ui.il_opacity->setValue(lb->getOpacity());
	ui.il_opacity_bod->setValue(lb->getOpacity_BG());

	ui.il_fontBox->setFont(QFont(QString::fromStdString(lb->getFont())));
	ui.il_prev->setFont(QFont(QString::fromStdString(lb->getFont()), lb->getSize()));

	pal = ui.il_prev->palette();
	pal.setColor(ui.il_prev->foregroundRole(), qRgb(lb->getRed(), lb->getGreen(), lb->getBlue()));
	color = pal.color(ui.il_prev->foregroundRole());
	color.setAlpha(lb->getOpacity());
	pal.setColor(ui.il_prev->foregroundRole(), color);

	pal.setColor(ui.il_prev->backgroundRole(), qRgb(lb->getRed_BG(), lb->getGreen_BG(), lb->getBlue_BG()));
	color = pal.color(ui.il_prev->backgroundRole());
	color.setAlpha(lb->getOpacity_BG());
	pal.setColor(ui.il_prev->backgroundRole(), color);
	ui.il_prev->setPalette(pal);

	ui.il_width->setValue(lb->getWidth());
	ui.il_height->setValue(lb->getHeight());

	ui.il_prev->setFixedHeight(lb->getHeight());
	ui.il_prev->setFixedWidth(lb->getWidth());
}

StyleSettings::~StyleSettings()
{
}

// HEADER 1
void StyleSettings::pickFontOpacity_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setOpacity(ui.h1_opacity->value());

	QPalette pal = ui.header1_prev->palette();
	//pal.setColor(ui.header1_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	QColor color = pal.color(ui.header1_prev->foregroundRole());
	color.setAlpha(ui.h1_opacity->value());
	pal.setColor(ui.header1_prev->foregroundRole(), color);
	ui.header1_prev->setPalette(pal);

	ui.h1_opacityBox->setValue(ui.h1_opacity->value());

	n->setH1(lb);
}

void StyleSettings::pickBGOpacity_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setOpacity_BG(ui.h1_opacity_bod->value());

	QPalette pal = ui.header1_prev->palette();
	//pal.setColor(ui.header1_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	QColor color = pal.color(ui.header1_prev->backgroundRole());
	color.setAlpha(ui.h1_opacity_bod->value());
	pal.setColor(ui.header1_prev->backgroundRole(), color);
	ui.header1_prev->setPalette(pal);

	ui.h1_opacityBox_bod->setValue(ui.h1_opacity_bod->value());

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

void StyleSettings::pickHeight_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setHeight(ui.h1_height->value());
	ui.header1_prev->setFixedHeight(lb->getHeight());
	n->setH1(lb);
}

void StyleSettings::pickWidth_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setWidth(ui.h1_width->value());
	ui.header1_prev->setFixedWidth(lb->getWidth());
	n->setH1(lb);
}

void StyleSettings::pickFontOpacityBox_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setOpacity(ui.h1_opacityBox->value());

	QPalette pal = ui.header1_prev->palette();
	pal.setColor(ui.header1_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	QColor color = pal.color(ui.header1_prev->foregroundRole());
	color.setAlpha(ui.h1_opacityBox->value());
	pal.setColor(ui.header1_prev->foregroundRole(), color);
	ui.header1_prev->setPalette(pal);

	ui.h1_opacity->setValue(ui.h1_opacityBox->value());

	n->setH1(lb);
}


void StyleSettings::pickBGOpacityBox_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setOpacity_BG(ui.h1_opacityBox_bod->value());

	QPalette pal = ui.header1_prev->palette();
	pal.setColor(ui.header1_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	QColor color = pal.color(ui.header1_prev->backgroundRole());
	color.setAlpha(ui.h1_opacityBox_bod->value());
	pal.setColor(ui.header1_prev->backgroundRole(), color);
	ui.header1_prev->setPalette(pal);

	ui.h1_opacity_bod->setValue(ui.h1_opacityBox_bod->value());

	n->setH1(lb);
}

void StyleSettings::pickJustification_h1()
{
	LabelStyle* lb = n->getH1();
	QStringList justifications;
	justifications << "Left" << "Center" << "Right" << "Justify";

	lb->setAlign(justifications.at(ui.h1_justify->currentIndex()).toStdString());
	if (lb->getAlign() == "Left")
		ui.header1_prev->setAlignment(Qt::AlignLeft);
	else if (lb->getAlign() == "Right")
		ui.header1_prev->setAlignment(Qt::AlignRight);
	else if (lb->getAlign() == "Justify")
		ui.header1_prev->setAlignment(Qt::AlignJustify);
	else
		ui.header1_prev->setAlignment(Qt::AlignHCenter);

	n->setH1(lb);
}

void StyleSettings::pickStyle_h1()
{
	LabelStyle* lb = n->getH1();
	QStringList styles;
	styles << "Regular" << "Bold" << "Italic";
	lb->setWeight(styles.at(ui.h1_styleBox->currentIndex()).toStdString());

	const QFont temp = ui.header1_prev->font();
	QFont temp2 = temp;
	if (lb->getWeight() == "Italic")
	{
		temp2.setItalic(true);
		temp2.setBold(false);
	}
	else if (lb->getWeight() == "Bold")
	{
		temp2.setBold(true);
		temp2.setItalic(false);
	}
	else
	{
		temp2.setBold(false);
		temp2.setItalic(false);
	}

	ui.header1_prev->setFont(temp2);
	n->setH1(lb);
}

void StyleSettings::pickMargin_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setMargin(ui.h1_margin->value());
	ui.header1_prev->setMargin(lb->getMargin());
	n->setH1(lb);
}

// HEADER 2
void StyleSettings::pickFontOpacity_h2()
{
	LabelStyle* lb = n->getH2();
	lb->setOpacity(ui.h2_opacity->value());

	QPalette pal = ui.header2_prev->palette();
	pal.setColor(ui.header2_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	QColor color = pal.color(ui.header2_prev->foregroundRole());
	color.setAlpha(ui.h2_opacity->value());
	pal.setColor(ui.header2_prev->foregroundRole(), color);
	ui.header2_prev->setPalette(pal);

	n->setH2(lb);
}

void StyleSettings::pickBGOpacity_h2()
{
	LabelStyle* lb = n->getH2();
	lb->setOpacity_BG(ui.h2_opacity_bod->value());

	QPalette pal = ui.header2_prev->palette();
	pal.setColor(ui.header2_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	QColor color = pal.color(ui.header2_prev->backgroundRole());
	color.setAlpha(ui.h2_opacity_bod->value());
	pal.setColor(ui.header2_prev->backgroundRole(), color);
	ui.header2_prev->setPalette(pal);

	n->setH2(lb);
}

void StyleSettings::pickFontSize_h2()
{
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";

	LabelStyle* lb = n->getH2();
	lb->setSize(fontSizes.at(ui.h2_sizeBox->currentIndex()).toInt());
	const QFont temp = ui.header2_prev->font();
	QFont temp2 = temp;
	temp2.setPointSize(fontSizes.at(ui.h2_sizeBox->currentIndex()).toInt());
	ui.header2_prev->setFont(temp2);
	//ui.header1_prev->setStyleSheet(QString::fromStdString("font-size:" + std::to_string(fontSizes.at(ui.h1_sizeBox->currentIndex()).toInt()) + "pt;"));
	n->setH2(lb);
}

void StyleSettings::pickFontColor_h2()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.h2_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));
	//ui.header1_prev->setStyleSheet(QString::fromStdString("color:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.header2_prev->palette();
	pal.setColor(ui.header2_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	ui.header2_prev->setPalette(pal);

	LabelStyle* lb = n->getH2();
	lb->setRed(color.red());
	lb->setBlue(color.blue());
	lb->setGreen(color.green());

	n->setH2(lb);
}

void StyleSettings::pickBGColor_h2()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	bgColor = dlg.selectedColor();
	ui.h2_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(bgColor.red()) + "," + std::to_string(bgColor.green()) + "," + std::to_string(bgColor.blue()) + ");"));
	//ui.header1_prev->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.header2_prev->palette();
	pal.setColor(ui.header2_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	ui.header2_prev->setPalette(pal);

	LabelStyle* lb = n->getH2();
	lb->setRed_BG(bgColor.red());
	lb->setBlue_BG(bgColor.blue());
	lb->setGreen_BG(bgColor.green());

	n->setH2(lb);
}

void StyleSettings::pickFont_h2()
{
	LabelStyle* lb = n->getH2();
	lb->setFont(ui.h2_fontBox->currentFont().family().toStdString());
	const QFont temp = ui.header2_prev->font();
	QFont temp2 = temp;
	temp2.setFamily(ui.h2_fontBox->currentFont().family());
	ui.header2_prev->setFont(temp2);
	//ui.header1_prev->setStyleSheet(QString::fromStdString("font-family:" + ui.h1_fontBox->currentFont().family().toStdString() + ";"));
	n->setH2(lb);
}


// BODY TEXT
void StyleSettings::pickFontOpacity_bt()
{
	LabelStyle* lb = n->getBod();
	lb->setOpacity(ui.bt_opacity->value());

	QPalette pal = ui.bt_prev->palette();
	pal.setColor(ui.bt_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	QColor color = pal.color(ui.bt_prev->foregroundRole());
	color.setAlpha(ui.bt_opacity->value());
	pal.setColor(ui.bt_prev->foregroundRole(), color);
	ui.bt_prev->setPalette(pal);

	n->setBod(lb);
}

void StyleSettings::pickBGOpacity_bt()
{
	LabelStyle* lb = n->getBod();
	lb->setOpacity_BG(ui.bt_opacity_bod->value());

	QPalette pal = ui.bt_prev->palette();
	pal.setColor(ui.bt_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	QColor color = pal.color(ui.bt_prev->backgroundRole());
	color.setAlpha(ui.bt_opacity_bod->value());
	pal.setColor(ui.bt_prev->backgroundRole(), color);
	ui.bt_prev->setPalette(pal);

	n->setBod(lb);
}

void StyleSettings::pickFontSize_bt()
{
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";

	LabelStyle* lb = n->getBod();
	lb->setSize(fontSizes.at(ui.bt_sizeBox->currentIndex()).toInt());
	const QFont temp = ui.bt_prev->font();
	QFont temp2 = temp;
	temp2.setPointSize(fontSizes.at(ui.bt_sizeBox->currentIndex()).toInt());
	ui.bt_prev->setFont(temp2);
	//ui.header1_prev->setStyleSheet(QString::fromStdString("font-size:" + std::to_string(fontSizes.at(ui.h1_sizeBox->currentIndex()).toInt()) + "pt;"));
	n->setBod(lb);
}

void StyleSettings::pickFontColor_bt()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.bt_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));
	//ui.header1_prev->setStyleSheet(QString::fromStdString("color:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.bt_prev->palette();
	pal.setColor(ui.bt_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	ui.bt_prev->setPalette(pal);

	LabelStyle* lb = n->getBod();
	lb->setRed(color.red());
	lb->setBlue(color.blue());
	lb->setGreen(color.green());

	n->setBod(lb);
}

void StyleSettings::pickBGColor_bt()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	bgColor = dlg.selectedColor();
	ui.bt_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(bgColor.red()) + "," + std::to_string(bgColor.green()) + "," + std::to_string(bgColor.blue()) + ");"));
	//ui.header1_prev->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.bt_prev->palette();
	pal.setColor(ui.bt_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	ui.bt_prev->setPalette(pal);

	LabelStyle* lb = n->getBod();
	lb->setRed_BG(bgColor.red());
	lb->setBlue_BG(bgColor.blue());
	lb->setGreen_BG(bgColor.green());

	n->setBod(lb);
}

void StyleSettings::pickFont_bt()
{
	LabelStyle* lb = n->getBod();
	lb->setFont(ui.bt_fontBox->currentFont().family().toStdString());
	const QFont temp = ui.bt_prev->font();
	QFont temp2 = temp;
	temp2.setFamily(ui.bt_fontBox->currentFont().family());
	ui.bt_prev->setFont(temp2);
	//ui.header1_prev->setStyleSheet(QString::fromStdString("font-family:" + ui.h1_fontBox->currentFont().family().toStdString() + ";"));
	n->setBod(lb);
}


//LABEL TEXT
void StyleSettings::pickFontOpacity_lt()
{
	LabelStyle* lb = n->getLab();
	lb->setOpacity(ui.lt_opacity->value());

	QPalette pal = ui.lt_prev->palette();
	pal.setColor(ui.lt_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	QColor color = pal.color(ui.lt_prev->foregroundRole());
	color.setAlpha(ui.lt_opacity->value());
	pal.setColor(ui.lt_prev->foregroundRole(), color);
	ui.lt_prev->setPalette(pal);

	n->setLab(lb);
}

void StyleSettings::pickBGOpacity_lt()
{
	LabelStyle* lb = n->getLab();
	lb->setOpacity_BG(ui.lt_opacity_bod->value());

	QPalette pal = ui.lt_prev->palette();
	pal.setColor(ui.lt_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	QColor color = pal.color(ui.lt_prev->backgroundRole());
	color.setAlpha(ui.lt_opacity_bod->value());
	pal.setColor(ui.lt_prev->backgroundRole(), color);
	ui.lt_prev->setPalette(pal);

	n->setLab(lb);
}

void StyleSettings::pickFontSize_lt()
{
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";

	LabelStyle* lb = n->getLab();
	lb->setSize(fontSizes.at(ui.lt_sizeBox->currentIndex()).toInt());
	const QFont temp = ui.lt_prev->font();
	QFont temp2 = temp;
	temp2.setPointSize(fontSizes.at(ui.lt_sizeBox->currentIndex()).toInt());
	ui.lt_prev->setFont(temp2);
	//ui.header1_prev->setStyleSheet(QString::fromStdString("font-size:" + std::to_string(fontSizes.at(ui.h1_sizeBox->currentIndex()).toInt()) + "pt;"));
	n->setLab(lb);
}

void StyleSettings::pickFontColor_lt()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.lt_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));
	//ui.header1_prev->setStyleSheet(QString::fromStdString("color:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.lt_prev->palette();
	pal.setColor(ui.lt_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	ui.lt_prev->setPalette(pal);

	LabelStyle* lb = n->getLab();
	lb->setRed(color.red());
	lb->setBlue(color.blue());
	lb->setGreen(color.green());

	n->setLab(lb);
}

void StyleSettings::pickBGColor_lt()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	bgColor = dlg.selectedColor();
	ui.lt_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(bgColor.red()) + "," + std::to_string(bgColor.green()) + "," + std::to_string(bgColor.blue()) + ");"));
	//ui.header1_prev->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.lt_prev->palette();
	pal.setColor(ui.lt_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	ui.lt_prev->setPalette(pal);

	LabelStyle* lb = n->getLab();
	lb->setRed_BG(bgColor.red());
	lb->setBlue_BG(bgColor.blue());
	lb->setGreen_BG(bgColor.green());

	n->setLab(lb);
}

void StyleSettings::pickFont_lt()
{
	LabelStyle* lb = n->getLab();
	lb->setFont(ui.lt_fontBox->currentFont().family().toStdString());
	const QFont temp = ui.lt_prev->font();
	QFont temp2 = temp;
	temp2.setFamily(ui.lt_fontBox->currentFont().family());
	ui.lt_prev->setFont(temp2);
	//ui.header1_prev->setStyleSheet(QString::fromStdString("font-family:" + ui.h1_fontBox->currentFont().family().toStdString() + ";"));
	n->setLab(lb);
}

//IMAGE TEXT
void StyleSettings::pickFontOpacity_il()
{
	LabelStyle* lb = n->getImg();
	lb->setOpacity(ui.il_opacity->value());

	QPalette pal = ui.il_prev->palette();
	pal.setColor(ui.il_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	QColor color = pal.color(ui.il_prev->foregroundRole());
	color.setAlpha(ui.il_opacity->value());
	pal.setColor(ui.il_prev->foregroundRole(), color);
	ui.il_prev->setPalette(pal);

	n->setImg(lb);
}

void StyleSettings::pickBGOpacity_il()
{
	LabelStyle* lb = n->getImg();
	lb->setOpacity_BG(ui.il_opacity_bod->value());

	QPalette pal = ui.il_prev->palette();
	pal.setColor(ui.il_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	QColor color = pal.color(ui.il_prev->backgroundRole());
	color.setAlpha(ui.il_opacity_bod->value());
	pal.setColor(ui.il_prev->backgroundRole(), color);
	ui.il_prev->setPalette(pal);

	n->setImg(lb);
}

void StyleSettings::pickFontSize_il()
{
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";

	LabelStyle* lb = n->getImg();
	lb->setSize(fontSizes.at(ui.il_sizeBox->currentIndex()).toInt());
	const QFont temp = ui.il_prev->font();
	QFont temp2 = temp;
	temp2.setPointSize(fontSizes.at(ui.il_sizeBox->currentIndex()).toInt());
	ui.il_prev->setFont(temp2);
	//ui.header1_prev->setStyleSheet(QString::fromStdString("font-size:" + std::to_string(fontSizes.at(ui.h1_sizeBox->currentIndex()).toInt()) + "pt;"));
	n->setImg(lb);
}

void StyleSettings::pickFontColor_il()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.il_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));
	//ui.header1_prev->setStyleSheet(QString::fromStdString("color:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.il_prev->palette();
	pal.setColor(ui.il_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	ui.il_prev->setPalette(pal);

	LabelStyle* lb = n->getImg();
	lb->setRed(color.red());
	lb->setBlue(color.blue());
	lb->setGreen(color.green());

	n->setImg(lb);
}

void StyleSettings::pickBGColor_il()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	bgColor = dlg.selectedColor();
	ui.il_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(bgColor.red()) + "," + std::to_string(bgColor.green()) + "," + std::to_string(bgColor.blue()) + ");"));
	//ui.header1_prev->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.il_prev->palette();
	pal.setColor(ui.il_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	ui.il_prev->setPalette(pal);

	LabelStyle* lb = n->getImg();
	lb->setRed_BG(bgColor.red());
	lb->setBlue_BG(bgColor.blue());
	lb->setGreen_BG(bgColor.green());

	n->setImg(lb);
}

void StyleSettings::pickFont_il()
{
	LabelStyle* lb = n->getImg();
	lb->setFont(ui.il_fontBox->currentFont().family().toStdString());
	const QFont temp = ui.il_prev->font();
	QFont temp2 = temp;
	temp2.setFamily(ui.il_fontBox->currentFont().family());
	ui.il_prev->setFont(temp2);
	//ui.header1_prev->setStyleSheet(QString::fromStdString("font-family:" + ui.h1_fontBox->currentFont().family().toStdString() + ";"));
	n->setImg(lb);
}