#include "StyleSettingsWidget.h"

#include <QColorDialog>
#include <QFontDatabase>

#include "LabelStyle.h"
#include "Util.h"

#include <QDebug>

StyleSettingsWidget::StyleSettingsWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_font_sizes = QFontDatabase::standardSizes();
	QStringList size_strings;
	for (int s : m_font_sizes) {
		size_strings.append(QString::number(s));
	}
	ui.sizeBox->addItems(size_strings);

	QStringList justifications;
	justifications << "Left" << "Center" << "Right" << "Justify";
	ui.justify->addItems(justifications);

	connect(ui.colorPicker, &QPushButton::clicked, this,
		[this]() {
		QColorDialog dlg;
		dlg.setOption(QColorDialog::ShowAlphaChannel, true);
		dlg.setCurrentColor(m_color);
		int result = dlg.exec();
		if (result == QDialog::Rejected) {
			return;
		}
		m_color = dlg.selectedColor();
		qDebug() << "color to stylesheet" << Util::colorToStylesheet(m_color);
		ui.colorPicker->setStyleSheet(Util::colorToStylesheet(m_color));
		
		// update the preview
		ui.prev->setStyleSheet(Util::colorToStylesheet(m_color));
	});
	connect(ui.fontBox, &QFontComboBox::currentFontChanged, this,
		[this](const QFont &f) {
		ui.prev->setFont(f);
	});
	connect(ui.sizeBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
		[this](int index) {
		QFont f = ui.prev->font();
		int new_size = m_font_sizes[index];
		f.setPixelSize(new_size);
		ui.prev->setFont(f);
	});

	//connect(ui.colorPicker_bod, &QPushButton::clicked, this, &StyleSettingsWidget::pickBGColor_h1);
	//connect(ui.fontBox, &QFontComboBox::currentFontChanged, this, &StyleSettingsWidget::pickFont_h1);
	//connect(ui.sizeBox, &QComboBox::currentTextChanged, this, &StyleSettingsWidget::pickFontSize_h1);
	//connect(ui.opacity, &QSlider::valueChanged, this, &StyleSettingsWidget::pickFontOpacity_h1);
	//connect(ui.opacity_bod, &QSlider::valueChanged, this, &StyleSettingsWidget::pickBGOpacity_h1);
	//connect(ui.opacityBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &StyleSettingsWidget::pickFontOpacityBox_h1);
	//connect(ui.opacityBox_bod, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &StyleSettingsWidget::pickBGOpacityBox_h1);
	//connect(ui.width, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &StyleSettingsWidget::pickWidth_h1);
	//connect(ui.height, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &StyleSettingsWidget::pickHeight_h1);
	//connect(ui.styleBox, &QComboBox::currentTextChanged, this, &StyleSettingsWidget::pickStyle_h1);
	//connect(ui.justify, &QComboBox::currentTextChanged, this, &StyleSettingsWidget::pickJustification_h1);
	//connect(ui.margin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &StyleSettingsWidget::pickMargin_h1);

	//ui.header1_prev->setAutoFillBackground(true);

}
//
//StyleSettingsWidget::StyleSettingsWidget(Narrative2* narr, QWidget *parent)
//	: StyleSettingsWidget(parent)
//{
//	n = narr;
//
//	//header 1
//#ifdef STYLE_SETTINGS
//	LabelStyle* lb = n->getH1();
//	ui.h1_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + ");"));
//	ui.h1_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + ");"));
//
//	QStringList fontSizes;
//	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";
//	ui.h1_sizeBox->setCurrentIndex(fontSizes.lastIndexOf(QString::number(lb->getSize())));
//	ui.h1_opacity->setValue(lb->getOpacity());
//	ui.h1_opacity_bod->setValue(lb->getOpacity_BG());
//
//	ui.h1_opacityBox->setValue(float(lb->getOpacity()) / 255 * 100);
//	ui.h1_opacityBox_bod->setValue(float(lb->getOpacity_BG()) / 255 * 100);
//
//	ui.h1_fontBox->setCurrentText(QString::fromStdString(lb->getFont()));
//	ui.header1_prev->setFont(QFont(QString::fromStdString(lb->getFont()), lb->getSize()));
//
//	QStringList styles;
//	styles << "Regular" << "Bold" << "Italic";
//	ui.h1_styleBox->setCurrentIndex(styles.lastIndexOf(QString::fromStdString(lb->getWeight())));
//
//	const QFont temp = ui.header1_prev->font();
//	QFont temp2 = temp;
//	if (lb->getWeight() == "Italic")
//	{
//		temp2.setItalic(true);
//		temp2.setBold(false);
//	}
//	else if (lb->getWeight() == "Bold")
//	{
//		temp2.setBold(true);
//		temp2.setItalic(false);
//	}
//	else
//	{
//		temp2.setBold(false);
//		temp2.setItalic(false);
//	}
//
//	ui.header1_prev->setFont(temp2);
//
//	QStringList justifications;
//	justifications << "Left" << "Center" << "Right" << "Justify";
//	ui.h1_justify->setCurrentIndex(justifications.lastIndexOf(QString::fromStdString(lb->getAlign())));
//
//	if (lb->getAlign() == "Left")
//		ui.header1_prev->setAlignment(Qt::AlignLeft);
//	else if (lb->getAlign() == "Right")
//		ui.header1_prev->setAlignment(Qt::AlignRight);
//	else if (lb->getAlign() == "Justify")
//		ui.header1_prev->setAlignment(Qt::AlignJustify);
//	else
//		ui.header1_prev->setAlignment(Qt::AlignHCenter);
//
//	ui.header1_prev->setMargin(lb->getMargin());
//	ui.h1_margin->setValue(lb->getMargin());
//
//	QPalette pal = ui.header1_prev->palette();
//	pal.setColor(ui.header1_prev->foregroundRole(), qRgb(lb->getRed(), lb->getGreen(), lb->getBlue()));
//	QColor color = pal.color(ui.header1_prev->foregroundRole());
//	color.setAlpha(lb->getOpacity());
//	pal.setColor(ui.header1_prev->foregroundRole(), color);
//
//	pal.setColor(ui.header1_prev->backgroundRole(), qRgb(lb->getRed_BG(), lb->getGreen_BG(), lb->getBlue_BG()));
//	color = pal.color(ui.header1_prev->backgroundRole());
//	color.setAlpha(lb->getOpacity_BG());
//	pal.setColor(ui.header1_prev->backgroundRole(), color);
//	ui.header1_prev->setPalette(pal);
//
//	ui.h1_width->setValue(lb->getWidth());
//	ui.h1_height->setValue(lb->getHeight());
//
//	ui.header1_prev->setFixedHeight(lb->getHeight());
//	ui.header1_prev->setFixedWidth(lb->getWidth());
//
//	//header 2
//	LabelStyle* lb2 = n->getH2();
//	ui.h2_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb2->getRed()) + "," + std::to_string(lb2->getGreen()) + "," + std::to_string(lb2->getBlue()) + ");"));
//	ui.h2_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb2->getRed_BG()) + "," + std::to_string(lb2->getGreen_BG()) + "," + std::to_string(lb2->getBlue_BG()) + ");"));
//
//	ui.h2_sizeBox->setCurrentIndex(fontSizes.lastIndexOf(QString::number(lb2->getSize())));
//	ui.h2_opacity->setValue(lb2->getOpacity());
//	ui.h2_opacity_bod->setValue(lb2->getOpacity_BG());
//
//	ui.h2_opacityBox->setValue(float(lb2->getOpacity()) / 255 * 100);
//	ui.h2_opacityBox_bod->setValue(float(lb2->getOpacity_BG()) / 255 * 100);
//
//	ui.h2_fontBox->setCurrentText(QString::fromStdString(lb2->getFont()));
//	ui.header2_prev->setFont(QFont(QString::fromStdString(lb2->getFont()), lb2->getSize()));
//
//	ui.h2_styleBox->setCurrentIndex(styles.lastIndexOf(QString::fromStdString(lb2->getWeight())));
//
//	const QFont temp_2 = ui.header2_prev->font();
//	temp2 = temp_2;
//	if (lb2->getWeight() == "Italic")
//	{
//		temp2.setItalic(true);
//		temp2.setBold(false);
//	}
//	else if (lb2->getWeight() == "Bold")
//	{
//		temp2.setBold(true);
//		temp2.setItalic(false);
//	}
//	else
//	{
//		temp2.setBold(false);
//		temp2.setItalic(false);
//	}
//
//	ui.header2_prev->setFont(temp2);
//
//	ui.h2_justify->setCurrentIndex(justifications.lastIndexOf(QString::fromStdString(lb2->getAlign())));
//
//	if (lb2->getAlign() == "Left")
//		ui.header2_prev->setAlignment(Qt::AlignLeft);
//	else if (lb2->getAlign() == "Right")
//		ui.header2_prev->setAlignment(Qt::AlignRight);
//	else if (lb2->getAlign() == "Justify")
//		ui.header2_prev->setAlignment(Qt::AlignJustify);
//	else
//		ui.header2_prev->setAlignment(Qt::AlignHCenter);
//
//	ui.header2_prev->setMargin(lb2->getMargin());
//	ui.h2_margin->setValue(lb2->getMargin());
//
//	pal = ui.header2_prev->palette();
//	pal.setColor(ui.header2_prev->foregroundRole(), qRgb(lb2->getRed(), lb2->getGreen(), lb2->getBlue()));
//	color = pal.color(ui.header2_prev->foregroundRole());
//	color.setAlpha(lb2->getOpacity());
//	pal.setColor(ui.header2_prev->foregroundRole(), color);
//
//	pal.setColor(ui.header2_prev->backgroundRole(), qRgb(lb2->getRed_BG(), lb2->getGreen_BG(), lb2->getBlue_BG()));
//	color = pal.color(ui.header2_prev->backgroundRole());
//	color.setAlpha(lb2->getOpacity_BG());
//	pal.setColor(ui.header2_prev->backgroundRole(), color);
//	ui.header2_prev->setPalette(pal);
//
//	ui.h2_width->setValue(lb2->getWidth());
//	ui.h2_height->setValue(lb2->getHeight());
//
//	ui.header2_prev->setFixedHeight(lb2->getHeight());
//	ui.header2_prev->setFixedWidth(lb2->getWidth());
//
//	//body text
//	lb = n->getBod();
//	ui.bt_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + ");"));
//	ui.bt_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + ");"));
//
//	ui.bt_sizeBox->setCurrentIndex(fontSizes.lastIndexOf(QString::number(lb->getSize())));
//	ui.bt_opacity->setValue(lb->getOpacity());
//	ui.bt_opacity_bod->setValue(lb->getOpacity_BG());
//
//	ui.bt_opacityBox->setValue(float(lb->getOpacity()) / 255 * 100);
//	ui.bt_opacityBox_bod->setValue(float(lb->getOpacity_BG()) / 255 * 100);
//
//	ui.bt_fontBox->setCurrentText(QString::fromStdString(lb->getFont()));
//	ui.bt_prev->setFont(QFont(QString::fromStdString(lb->getFont()), lb->getSize()));
//
//	ui.bt_styleBox->setCurrentIndex(styles.lastIndexOf(QString::fromStdString(lb->getWeight())));
//
//	const QFont temp_3 = ui.bt_prev->font();
//	temp2 = temp_3;
//	if (lb->getWeight() == "Italic")
//	{
//		temp2.setItalic(true);
//		temp2.setBold(false);
//	}
//	else if (lb->getWeight() == "Bold")
//	{
//		temp2.setBold(true);
//		temp2.setItalic(false);
//	}
//	else
//	{
//		temp2.setBold(false);
//		temp2.setItalic(false);
//	}
//
//	ui.bt_prev->setFont(temp2);
//
//	ui.bt_justify->setCurrentIndex(justifications.lastIndexOf(QString::fromStdString(lb->getAlign())));
//
//	if (lb->getAlign() == "Left")
//		ui.bt_prev->setAlignment(Qt::AlignLeft);
//	else if (lb->getAlign() == "Right")
//		ui.bt_prev->setAlignment(Qt::AlignRight);
//	else if (lb->getAlign() == "Justify")
//		ui.bt_prev->setAlignment(Qt::AlignJustify);
//	else
//		ui.bt_prev->setAlignment(Qt::AlignHCenter);
//
//	ui.bt_prev->setMargin(lb->getMargin());
//	ui.bt_margin->setValue(lb->getMargin());
//
//	pal = ui.bt_prev->palette();
//	pal.setColor(ui.bt_prev->foregroundRole(), qRgb(lb->getRed(), lb->getGreen(), lb->getBlue()));
//	color = pal.color(ui.bt_prev->foregroundRole());
//	color.setAlpha(lb->getOpacity());
//	pal.setColor(ui.bt_prev->foregroundRole(), color);
//
//	pal.setColor(ui.bt_prev->backgroundRole(), qRgb(lb->getRed_BG(), lb->getGreen_BG(), lb->getBlue_BG()));
//	color = pal.color(ui.bt_prev->backgroundRole());
//	color.setAlpha(lb->getOpacity_BG());
//	pal.setColor(ui.bt_prev->backgroundRole(), color);
//	ui.bt_prev->setPalette(pal);
//
//	ui.bt_width->setValue(lb->getWidth());
//	ui.bt_height->setValue(lb->getHeight());
//
//	ui.bt_prev->setFixedHeight(lb->getHeight());
//	ui.bt_prev->setFixedWidth(lb->getWidth());
//
//	//label text
//	lb = n->getLab();
//	ui.lt_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + ");"));
//	ui.lt_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + ");"));
//
//	ui.lt_sizeBox->setCurrentIndex(fontSizes.lastIndexOf(QString::number(lb->getSize())));
//	ui.lt_opacity->setValue(lb->getOpacity());
//	ui.lt_opacity_bod->setValue(lb->getOpacity_BG());
//
//	ui.lt_opacityBox->setValue(float(lb->getOpacity()) / 255 * 100);
//	ui.lt_opacityBox_bod->setValue(float(lb->getOpacity_BG()) / 255 * 100);
//
//	ui.lt_fontBox->setCurrentText(QString::fromStdString(lb->getFont()));
//	ui.lt_prev->setFont(QFont(QString::fromStdString(lb->getFont()), lb->getSize()));
//
//	ui.lt_styleBox->setCurrentIndex(styles.lastIndexOf(QString::fromStdString(lb->getWeight())));
//
//	const QFont temp_4 = ui.lt_prev->font();
//	temp2 = temp_4;
//	if (lb->getWeight() == "Italic")
//	{
//		temp2.setItalic(true);
//		temp2.setBold(false);
//	}
//	else if (lb->getWeight() == "Bold")
//	{
//		temp2.setBold(true);
//		temp2.setItalic(false);
//	}
//	else
//	{
//		temp2.setBold(false);
//		temp2.setItalic(false);
//	}
//
//	ui.lt_prev->setFont(temp2);
//
//	ui.lt_justify->setCurrentIndex(justifications.lastIndexOf(QString::fromStdString(lb->getAlign())));
//
//	if (lb->getAlign() == "Left")
//		ui.lt_prev->setAlignment(Qt::AlignLeft);
//	else if (lb->getAlign() == "Right")
//		ui.lt_prev->setAlignment(Qt::AlignRight);
//	else if (lb->getAlign() == "Justify")
//		ui.lt_prev->setAlignment(Qt::AlignJustify);
//	else
//		ui.lt_prev->setAlignment(Qt::AlignHCenter);
//
//	ui.lt_prev->setMargin(lb->getMargin());
//	ui.lt_margin->setValue(lb->getMargin());
//
//	pal = ui.lt_prev->palette();
//	pal.setColor(ui.lt_prev->foregroundRole(), qRgb(lb->getRed(), lb->getGreen(), lb->getBlue()));
//	color = pal.color(ui.lt_prev->foregroundRole());
//	color.setAlpha(lb->getOpacity());
//	pal.setColor(ui.lt_prev->foregroundRole(), color);
//
//	pal.setColor(ui.lt_prev->backgroundRole(), qRgb(lb->getRed_BG(), lb->getGreen_BG(), lb->getBlue_BG()));
//	color = pal.color(ui.lt_prev->backgroundRole());
//	color.setAlpha(lb->getOpacity_BG());
//	pal.setColor(ui.lt_prev->backgroundRole(), color);
//	ui.lt_prev->setPalette(pal);
//
//	ui.lt_width->setValue(lb->getWidth());
//	ui.lt_height->setValue(lb->getHeight());
//
//	ui.lt_prev->setFixedHeight(lb->getHeight());
//	ui.lt_prev->setFixedWidth(lb->getWidth());
//
//	//image label
//	lb = n->getImg();
//	ui.il_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + ");"));
//	ui.il_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + ");"));
//
//	ui.il_opacityBox->setValue(float(lb->getOpacity()) / 255 * 100);
//	ui.il_opacityBox_bod->setValue(float(lb->getOpacity_BG()) / 255 * 100);
//
//	ui.il_sizeBox->setCurrentIndex(fontSizes.lastIndexOf(QString::number(lb->getSize())));
//	ui.il_opacity->setValue(lb->getOpacity());
//	ui.il_opacity_bod->setValue(lb->getOpacity_BG());
//
//	ui.il_fontBox->setCurrentText(QString::fromStdString(lb->getFont()));
//	ui.il_prev->setFont(QFont(QString::fromStdString(lb->getFont()), lb->getSize()));
//
//	ui.il_styleBox->setCurrentIndex(styles.lastIndexOf(QString::fromStdString(lb->getWeight())));
//
//	const QFont temp_5 = ui.il_prev->font();
//	temp2 = temp_5;
//	if (lb->getWeight() == "Italic")
//	{
//		temp2.setItalic(true);
//		temp2.setBold(false);
//	}
//	else if (lb->getWeight() == "Bold")
//	{
//		temp2.setBold(true);
//		temp2.setItalic(false);
//	}
//	else
//	{
//		temp2.setBold(false);
//		temp2.setItalic(false);
//	}
//
//	ui.il_prev->setFont(temp2);
//
//	ui.il_justify->setCurrentIndex(justifications.lastIndexOf(QString::fromStdString(lb->getAlign())));
//
//	if (lb->getAlign() == "Left")
//		ui.il_prev->setAlignment(Qt::AlignLeft);
//	else if (lb->getAlign() == "Right")
//		ui.il_prev->setAlignment(Qt::AlignRight);
//	else if (lb->getAlign() == "Justify")
//		ui.il_prev->setAlignment(Qt::AlignJustify);
//	else
//		ui.il_prev->setAlignment(Qt::AlignHCenter);
//
//	ui.il_prev->setMargin(lb->getMargin());
//	ui.il_margin->setValue(lb->getMargin());
//
//	pal = ui.il_prev->palette();
//	pal.setColor(ui.il_prev->foregroundRole(), qRgb(lb->getRed(), lb->getGreen(), lb->getBlue()));
//	color = pal.color(ui.il_prev->foregroundRole());
//	color.setAlpha(lb->getOpacity());
//	pal.setColor(ui.il_prev->foregroundRole(), color);
//
//	pal.setColor(ui.il_prev->backgroundRole(), qRgb(lb->getRed_BG(), lb->getGreen_BG(), lb->getBlue_BG()));
//	color = pal.color(ui.il_prev->backgroundRole());
//	color.setAlpha(lb->getOpacity_BG());
//	pal.setColor(ui.il_prev->backgroundRole(), color);
//	ui.il_prev->setPalette(pal);
//
//	ui.il_width->setValue(lb->getWidth());
//	ui.il_height->setValue(lb->getHeight());
//
//	ui.il_prev->setFixedHeight(lb->getHeight());
//	ui.il_prev->setFixedWidth(lb->getWidth());
//#endif // STYLE_SETTINGS
//
//}

#ifdef StyleSettingsWidget

// HEADER 1
void StyleSettingsWidget::pickFontOpacity_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setOpacity(ui.h1_opacity->value());

	QPalette pal = ui.header1_prev->palette();
	color = pal.color(ui.header1_prev->foregroundRole());
	color.setAlpha(ui.h1_opacity->value());
	pal.setColor(ui.header1_prev->foregroundRole(), color);
	ui.header1_prev->setPalette(pal);
	float val = float(ui.h1_opacity->value()) / float(255) * float(100);
	ui.h1_opacityBox->setValue(std::round(val));

	n->setH1(lb);
}

void StyleSettingsWidget::pickBGOpacity_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setOpacity_BG(ui.h1_opacity_bod->value());

	QPalette pal = ui.header1_prev->palette();
	color = pal.color(ui.header1_prev->backgroundRole());
	color.setAlpha(ui.h1_opacity_bod->value());
	pal.setColor(ui.header1_prev->backgroundRole(), color);
	ui.header1_prev->setPalette(pal);
	float val = float(ui.h1_opacity_bod->value()) / float(255) * float(100);
	ui.h1_opacityBox_bod->setValue(std::round(val));

	n->setH1(lb);
}

void StyleSettingsWidget::pickFontOpacityBox_h1()
{
	LabelStyle* lb = n->getH1();
	float val = float(ui.h1_opacityBox->value()) / float(100) * float(255);
	lb->setOpacity(std::round(val));

	QPalette pal = ui.header1_prev->palette();
	color = pal.color(ui.header1_prev->foregroundRole());
	color.setAlpha(std::round(val));
	pal.setColor(ui.header1_prev->foregroundRole(), color);
	ui.header1_prev->setPalette(pal);

	ui.h1_opacity->setValue(std::round(val));

	n->setH1(lb);
}


void StyleSettingsWidget::pickBGOpacityBox_h1()
{
	LabelStyle* lb = n->getH1();
	float val = float(ui.h1_opacityBox_bod->value()) / float(100) * float(255);
	lb->setOpacity_BG(std::round(val));

	QPalette pal = ui.header1_prev->palette();
	color = pal.color(ui.header1_prev->backgroundRole());
	color.setAlpha(std::round(val));
	pal.setColor(ui.header1_prev->backgroundRole(), color);
	ui.header1_prev->setPalette(pal);

	ui.h1_opacity_bod->setValue(std::round(val));

	n->setH1(lb);
}

void StyleSettingsWidget::setStyle(const LabelStyle * style)
{
}

void StyleSettingsWidget::pickFontSize_h1()
{
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";

	LabelStyle* lb = n->getH1();
	lb->setSize(fontSizes.at(ui.h1_sizeBox->currentIndex()).toInt());
	const QFont temp = ui.header1_prev->font();
	QFont temp2 = temp;
	temp2.setPointSize(fontSizes.at(ui.h1_sizeBox->currentIndex()).toInt());
	ui.header1_prev->setFont(temp2);

	n->setH1(lb);
}

void StyleSettingsWidget::pickFontColor_h1()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.h1_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.header1_prev->palette();
	pal.setColor(ui.header1_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	ui.header1_prev->setPalette(pal);

	LabelStyle* lb = n->getH1();
	lb->setRed(color.red());
	lb->setBlue(color.blue());
	lb->setGreen(color.green());

	n->setH1(lb);

	pickFontOpacity_h1();
}

void StyleSettingsWidget::pickBGColor_h1()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	bgColor = dlg.selectedColor();
	ui.h1_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(bgColor.red()) + "," + std::to_string(bgColor.green()) + "," + std::to_string(bgColor.blue()) + ");"));

	QPalette pal = ui.header1_prev->palette();
	pal.setColor(ui.header1_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	ui.header1_prev->setPalette(pal);

	LabelStyle* lb = n->getH1();
	lb->setRed_BG(bgColor.red());
	lb->setBlue_BG(bgColor.blue());
	lb->setGreen_BG(bgColor.green());

	n->setH1(lb);

	pickBGOpacity_h1();
}

void StyleSettingsWidget::pickFont_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setFont(ui.h1_fontBox->currentFont().family().toStdString());
	const QFont temp = ui.header1_prev->font();
	QFont temp2 = temp;
	temp2.setFamily(ui.h1_fontBox->currentFont().family());
	ui.header1_prev->setFont(temp2);
	n->setH1(lb);
}

void StyleSettingsWidget::pickHeight_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setHeight(ui.h1_height->value());
	ui.header1_prev->setFixedHeight(lb->getHeight());
	n->setH1(lb);
}

void StyleSettingsWidget::pickWidth_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setWidth(ui.h1_width->value());
	ui.header1_prev->setFixedWidth(lb->getWidth());
	n->setH1(lb);
}

void StyleSettingsWidget::pickJustification_h1()
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

void StyleSettingsWidget::pickStyle_h1()
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

void StyleSettingsWidget::pickMargin_h1()
{
	LabelStyle* lb = n->getH1();
	lb->setMargin(ui.h1_margin->value());
	ui.header1_prev->setMargin(lb->getMargin());
	n->setH1(lb);
}

// HEADER 2
void StyleSettingsWidget::pickFontOpacity_h2()
{
	LabelStyle* lb = n->getH2();
	lb->setOpacity(ui.h2_opacity->value());

	QPalette pal = ui.header2_prev->palette();
	color = pal.color(ui.header2_prev->foregroundRole());
	color.setAlpha(ui.h2_opacity->value());
	pal.setColor(ui.header2_prev->foregroundRole(), color);
	ui.header2_prev->setPalette(pal);
	float val = float(ui.h2_opacity->value()) / float(255) * float(100);
	ui.h2_opacityBox->setValue(std::round(val));

	n->setH2(lb);
}

void StyleSettingsWidget::pickBGOpacity_h2()
{
	LabelStyle* lb = n->getH2();
	lb->setOpacity_BG(ui.h2_opacity_bod->value());

	QPalette pal = ui.header2_prev->palette();
	color = pal.color(ui.header2_prev->backgroundRole());
	color.setAlpha(ui.h2_opacity_bod->value());
	pal.setColor(ui.header2_prev->backgroundRole(), color);
	ui.header2_prev->setPalette(pal);
	float val = float(ui.h2_opacity_bod->value()) / float(255) * float(100);
	ui.h2_opacityBox_bod->setValue(std::round(val));

	n->setH2(lb);
}

void StyleSettingsWidget::pickFontOpacityBox_h2()
{
	LabelStyle* lb = n->getH2();
	float val = float(ui.h2_opacityBox->value()) / float(100) * float(255);
	lb->setOpacity(std::round(val));

	QPalette pal = ui.header2_prev->palette();
	color = pal.color(ui.header2_prev->foregroundRole());
	color.setAlpha(std::round(val));
	pal.setColor(ui.header2_prev->foregroundRole(), color);
	ui.header2_prev->setPalette(pal);

	ui.h2_opacity->setValue(std::round(val));

	n->setH2(lb);
}


void StyleSettingsWidget::pickBGOpacityBox_h2()
{
	LabelStyle* lb = n->getH2();
	float val = float(ui.h2_opacityBox_bod->value()) / float(100) * float(255);
	lb->setOpacity_BG(std::round(val));

	QPalette pal = ui.header2_prev->palette();
	color = pal.color(ui.header2_prev->backgroundRole());
	color.setAlpha(std::round(val));
	pal.setColor(ui.header2_prev->backgroundRole(), color);
	ui.header2_prev->setPalette(pal);

	ui.h2_opacity_bod->setValue(std::round(val));

	n->setH2(lb);
}

void StyleSettingsWidget::pickFontSize_h2()
{
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";

	LabelStyle* lb = n->getH2();
	lb->setSize(fontSizes.at(ui.h2_sizeBox->currentIndex()).toInt());
	const QFont temp = ui.header2_prev->font();
	QFont temp2 = temp;
	temp2.setPointSize(fontSizes.at(ui.h2_sizeBox->currentIndex()).toInt());
	ui.header2_prev->setFont(temp2);

	n->setH2(lb);
}

void StyleSettingsWidget::pickFontColor_h2()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.h2_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.header2_prev->palette();
	pal.setColor(ui.header2_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	ui.header2_prev->setPalette(pal);

	LabelStyle* lb = n->getH2();
	lb->setRed(color.red());
	lb->setBlue(color.blue());
	lb->setGreen(color.green());

	n->setH2(lb);

	pickFontOpacity_h2();
}

void StyleSettingsWidget::pickBGColor_h2()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	bgColor = dlg.selectedColor();
	ui.h2_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(bgColor.red()) + "," + std::to_string(bgColor.green()) + "," + std::to_string(bgColor.blue()) + ");"));

	QPalette pal = ui.header2_prev->palette();
	pal.setColor(ui.header2_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	ui.header2_prev->setPalette(pal);

	LabelStyle* lb = n->getH2();
	lb->setRed_BG(bgColor.red());
	lb->setBlue_BG(bgColor.blue());
	lb->setGreen_BG(bgColor.green());

	n->setH2(lb);

	pickBGOpacity_h2();
}

void StyleSettingsWidget::pickFont_h2()
{
	LabelStyle* lb = n->getH2();
	lb->setFont(ui.h2_fontBox->currentFont().family().toStdString());
	const QFont temp = ui.header2_prev->font();
	QFont temp2 = temp;
	temp2.setFamily(ui.h2_fontBox->currentFont().family());
	ui.header2_prev->setFont(temp2);

	n->setH2(lb);
}

void StyleSettingsWidget::pickHeight_h2()
{
	LabelStyle* lb = n->getH2();
	lb->setHeight(ui.h2_height->value());
	ui.header2_prev->setFixedHeight(lb->getHeight());
	n->setH2(lb);
}

void StyleSettingsWidget::pickWidth_h2()
{
	LabelStyle* lb = n->getH2();
	lb->setWidth(ui.h2_width->value());
	ui.header2_prev->setFixedWidth(lb->getWidth());
	n->setH2(lb);
}

void StyleSettingsWidget::pickJustification_h2()
{
	LabelStyle* lb = n->getH2();
	QStringList justifications;
	justifications << "Left" << "Center" << "Right" << "Justify";

	lb->setAlign(justifications.at(ui.h2_justify->currentIndex()).toStdString());
	if (lb->getAlign() == "Left")
		ui.header2_prev->setAlignment(Qt::AlignLeft);
	else if (lb->getAlign() == "Right")
		ui.header2_prev->setAlignment(Qt::AlignRight);
	else if (lb->getAlign() == "Justify")
		ui.header2_prev->setAlignment(Qt::AlignJustify);
	else
		ui.header2_prev->setAlignment(Qt::AlignHCenter);

	n->setH2(lb);
}

void StyleSettingsWidget::pickStyle_h2()
{
	LabelStyle* lb = n->getH2();
	QStringList styles;
	styles << "Regular" << "Bold" << "Italic";
	lb->setWeight(styles.at(ui.h2_styleBox->currentIndex()).toStdString());

	const QFont temp = ui.header2_prev->font();
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

	ui.header2_prev->setFont(temp2);
	n->setH2(lb);
}

void StyleSettingsWidget::pickMargin_h2()
{
	LabelStyle* lb = n->getH2();
	lb->setMargin(ui.h2_margin->value());
	ui.header2_prev->setMargin(lb->getMargin());
	n->setH2(lb);
}


// BODY TEXT
void StyleSettingsWidget::pickFontOpacity_bt()
{
	LabelStyle* lb = n->getBod();
	lb->setOpacity(ui.bt_opacity->value());

	QPalette pal = ui.bt_prev->palette();
	color = pal.color(ui.bt_prev->foregroundRole());
	color.setAlpha(ui.bt_opacity->value());
	pal.setColor(ui.bt_prev->foregroundRole(), color);
	ui.bt_prev->setPalette(pal);
	float val = float(ui.bt_opacity->value()) / float(255) * float(100);
	ui.bt_opacityBox->setValue(std::round(val));

	n->setBod(lb);
}

void StyleSettingsWidget::pickBGOpacity_bt()
{
	LabelStyle* lb = n->getBod();
	lb->setOpacity_BG(ui.bt_opacity_bod->value());

	QPalette pal = ui.bt_prev->palette();
	color = pal.color(ui.bt_prev->backgroundRole());
	color.setAlpha(ui.bt_opacity_bod->value());
	pal.setColor(ui.bt_prev->backgroundRole(), color);
	ui.bt_prev->setPalette(pal);
	float val = float(ui.bt_opacity_bod->value()) / float(255) * float(100);
	ui.bt_opacityBox_bod->setValue(std::round(val));

	n->setBod(lb);
}

void StyleSettingsWidget::pickFontOpacityBox_bt()
{
	LabelStyle* lb = n->getBod();
	float val = float(ui.bt_opacityBox->value()) / float(100) * float(255);
	lb->setOpacity(std::round(val));

	QPalette pal = ui.bt_prev->palette();
	color = pal.color(ui.bt_prev->foregroundRole());
	color.setAlpha(std::round(val));
	pal.setColor(ui.bt_prev->foregroundRole(), color);
	ui.bt_prev->setPalette(pal);

	ui.bt_opacity->setValue(std::round(val));

	n->setBod(lb);
}


void StyleSettingsWidget::pickBGOpacityBox_bt()
{
	LabelStyle* lb = n->getBod();
	float val = float(ui.bt_opacityBox_bod->value()) / float(100) * float(255);
	lb->setOpacity_BG(std::round(val));

	QPalette pal = ui.bt_prev->palette();
	color = pal.color(ui.bt_prev->backgroundRole());
	color.setAlpha(std::round(val));
	pal.setColor(ui.bt_prev->backgroundRole(), color);
	ui.bt_prev->setPalette(pal);

	ui.bt_opacity_bod->setValue(std::round(val));

	n->setBod(lb);
}

void StyleSettingsWidget::pickFontSize_bt()
{
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";

	LabelStyle* lb = n->getBod();
	lb->setSize(fontSizes.at(ui.bt_sizeBox->currentIndex()).toInt());
	const QFont temp = ui.bt_prev->font();
	QFont temp2 = temp;
	temp2.setPointSize(fontSizes.at(ui.bt_sizeBox->currentIndex()).toInt());
	ui.bt_prev->setFont(temp2);

	n->setBod(lb);
}

void StyleSettingsWidget::pickFontColor_bt()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.bt_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.bt_prev->palette();
	pal.setColor(ui.bt_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	ui.bt_prev->setPalette(pal);

	LabelStyle* lb = n->getBod();
	lb->setRed(color.red());
	lb->setBlue(color.blue());
	lb->setGreen(color.green());

	n->setBod(lb);

	pickFontOpacity_bt();
}

void StyleSettingsWidget::pickBGColor_bt()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	bgColor = dlg.selectedColor();
	ui.bt_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(bgColor.red()) + "," + std::to_string(bgColor.green()) + "," + std::to_string(bgColor.blue()) + ");"));

	QPalette pal = ui.bt_prev->palette();
	pal.setColor(ui.bt_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	ui.bt_prev->setPalette(pal);

	LabelStyle* lb = n->getBod();
	lb->setRed_BG(bgColor.red());
	lb->setBlue_BG(bgColor.blue());
	lb->setGreen_BG(bgColor.green());

	n->setBod(lb);

	pickBGOpacity_bt();
}

void StyleSettingsWidget::pickFont_bt()
{
	LabelStyle* lb = n->getBod();
	lb->setFont(ui.bt_fontBox->currentFont().family().toStdString());
	const QFont temp = ui.bt_prev->font();
	QFont temp2 = temp;
	temp2.setFamily(ui.bt_fontBox->currentFont().family());
	ui.bt_prev->setFont(temp2);

	n->setBod(lb);
}

void StyleSettingsWidget::pickHeight_bt()
{
	LabelStyle* lb = n->getBod();
	lb->setHeight(ui.bt_height->value());
	ui.bt_prev->setFixedHeight(lb->getHeight());
	n->setBod(lb);
}

void StyleSettingsWidget::pickWidth_bt()
{
	LabelStyle* lb = n->getBod();
	lb->setWidth(ui.bt_width->value());
	ui.bt_prev->setFixedWidth(lb->getWidth());
	n->setBod(lb);
}

void StyleSettingsWidget::pickJustification_bt()
{
	LabelStyle* lb = n->getBod();
	QStringList justifications;
	justifications << "Left" << "Center" << "Right" << "Justify";

	lb->setAlign(justifications.at(ui.bt_justify->currentIndex()).toStdString());
	if (lb->getAlign() == "Left")
		ui.bt_prev->setAlignment(Qt::AlignLeft);
	else if (lb->getAlign() == "Right")
		ui.bt_prev->setAlignment(Qt::AlignRight);
	else if (lb->getAlign() == "Justify")
		ui.bt_prev->setAlignment(Qt::AlignJustify);
	else
		ui.bt_prev->setAlignment(Qt::AlignHCenter);

	n->setBod(lb);
}

void StyleSettingsWidget::pickStyle_bt()
{
	LabelStyle* lb = n->getBod();
	QStringList styles;
	styles << "Regular" << "Bold" << "Italic";
	lb->setWeight(styles.at(ui.bt_styleBox->currentIndex()).toStdString());

	const QFont temp = ui.bt_prev->font();
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

	ui.bt_prev->setFont(temp2);
	n->setBod(lb);
}

void StyleSettingsWidget::pickMargin_bt()
{
	LabelStyle* lb = n->getBod();
	lb->setMargin(ui.bt_margin->value());
	ui.bt_prev->setMargin(lb->getMargin());
	n->setBod(lb);
}


//LABEL TEXT
void StyleSettingsWidget::pickFontOpacity_lt()
{
	LabelStyle* lb = n->getLab();
	lb->setOpacity(ui.lt_opacity->value());

	QPalette pal = ui.lt_prev->palette();
	color = pal.color(ui.lt_prev->foregroundRole());
	color.setAlpha(ui.lt_opacity->value());
	pal.setColor(ui.lt_prev->foregroundRole(), color);
	ui.lt_prev->setPalette(pal);
	float val = float(ui.lt_opacity->value()) / float(255) * float(100);
	ui.lt_opacityBox->setValue(std::round(val));

	n->setLab(lb);
}

void StyleSettingsWidget::pickBGOpacity_lt()
{
	LabelStyle* lb = n->getLab();
	lb->setOpacity_BG(ui.lt_opacity_bod->value());

	QPalette pal = ui.lt_prev->palette();
	QColor color = pal.color(ui.lt_prev->backgroundRole());
	color.setAlpha(ui.lt_opacity_bod->value());
	pal.setColor(ui.lt_prev->backgroundRole(), color);
	ui.lt_prev->setPalette(pal);
	float val = float(ui.lt_opacity_bod->value()) / float(255) * float(100);
	ui.lt_opacityBox_bod->setValue(std::round(val));

	n->setLab(lb);
}

void StyleSettingsWidget::pickFontOpacityBox_lt()
{
	LabelStyle* lb = n->getLab();
	float val = float(ui.lt_opacityBox->value()) / float(100) * float(255);
	lb->setOpacity(std::round(val));

	QPalette pal = ui.lt_prev->palette();
	color = pal.color(ui.lt_prev->foregroundRole());
	color.setAlpha(std::round(val));
	pal.setColor(ui.lt_prev->foregroundRole(), color);
	ui.lt_prev->setPalette(pal);
	ui.lt_opacity->setValue(std::round(val));

	n->setLab(lb);
}


void StyleSettingsWidget::pickBGOpacityBox_lt()
{
	LabelStyle* lb = n->getLab();
	float val = float(ui.lt_opacityBox_bod->value()) / float(100) * float(255);
	lb->setOpacity_BG(std::round(val));

	QPalette pal = ui.lt_prev->palette();
	color = pal.color(ui.lt_prev->backgroundRole());
	color.setAlpha(std::round(val));
	pal.setColor(ui.lt_prev->backgroundRole(), color);
	ui.lt_prev->setPalette(pal);
	ui.lt_opacity_bod->setValue(std::round(val));

	n->setLab(lb);
}

void StyleSettingsWidget::pickFontSize_lt()
{
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";

	LabelStyle* lb = n->getLab();
	lb->setSize(fontSizes.at(ui.lt_sizeBox->currentIndex()).toInt());
	const QFont temp = ui.lt_prev->font();
	QFont temp2 = temp;
	temp2.setPointSize(fontSizes.at(ui.lt_sizeBox->currentIndex()).toInt());
	ui.lt_prev->setFont(temp2);

	n->setLab(lb);
}

void StyleSettingsWidget::pickFontColor_lt()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.lt_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));


	QPalette pal = ui.lt_prev->palette();
	pal.setColor(ui.lt_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	ui.lt_prev->setPalette(pal);

	LabelStyle* lb = n->getLab();
	lb->setRed(color.red());
	lb->setBlue(color.blue());
	lb->setGreen(color.green());

	n->setLab(lb);

	pickFontOpacity_lt();
}

void StyleSettingsWidget::pickBGColor_lt()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	bgColor = dlg.selectedColor();
	ui.lt_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(bgColor.red()) + "," + std::to_string(bgColor.green()) + "," + std::to_string(bgColor.blue()) + ");"));

	QPalette pal = ui.lt_prev->palette();
	pal.setColor(ui.lt_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	ui.lt_prev->setPalette(pal);

	LabelStyle* lb = n->getLab();
	lb->setRed_BG(bgColor.red());
	lb->setBlue_BG(bgColor.blue());
	lb->setGreen_BG(bgColor.green());

	n->setLab(lb);

	pickBGOpacity_lt();
}

void StyleSettingsWidget::pickFont_lt()
{
	LabelStyle* lb = n->getLab();
	lb->setFont(ui.lt_fontBox->currentFont().family().toStdString());
	const QFont temp = ui.lt_prev->font();
	QFont temp2 = temp;
	temp2.setFamily(ui.lt_fontBox->currentFont().family());
	ui.lt_prev->setFont(temp2);

	n->setLab(lb);
}

void StyleSettingsWidget::pickHeight_lt()
{
	LabelStyle* lb = n->getLab();
	lb->setHeight(ui.lt_height->value());
	ui.lt_prev->setFixedHeight(lb->getHeight());
	n->setLab(lb);
}

void StyleSettingsWidget::pickWidth_lt()
{
	LabelStyle* lb = n->getLab();
	lb->setWidth(ui.lt_width->value());
	ui.lt_prev->setFixedWidth(lb->getWidth());
	n->setLab(lb);
}

void StyleSettingsWidget::pickJustification_lt()
{
	LabelStyle* lb = n->getLab();
	QStringList justifications;
	justifications << "Left" << "Center" << "Right" << "Justify";

	lb->setAlign(justifications.at(ui.lt_justify->currentIndex()).toStdString());
	if (lb->getAlign() == "Left")
		ui.lt_prev->setAlignment(Qt::AlignLeft);
	else if (lb->getAlign() == "Right")
		ui.lt_prev->setAlignment(Qt::AlignRight);
	else if (lb->getAlign() == "Justify")
		ui.lt_prev->setAlignment(Qt::AlignJustify);
	else
		ui.lt_prev->setAlignment(Qt::AlignHCenter);

	n->setLab(lb);
}

void StyleSettingsWidget::pickStyle_lt()
{
	LabelStyle* lb = n->getLab();
	QStringList styles;
	styles << "Regular" << "Bold" << "Italic";
	lb->setWeight(styles.at(ui.lt_styleBox->currentIndex()).toStdString());

	const QFont temp = ui.lt_prev->font();
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

	ui.lt_prev->setFont(temp2);
	n->setLab(lb);
}

void StyleSettingsWidget::pickMargin_lt()
{
	LabelStyle* lb = n->getLab();
	lb->setMargin(ui.lt_margin->value());
	ui.lt_prev->setMargin(lb->getMargin());
	n->setLab(lb);
}

//IMAGE TEXT
void StyleSettingsWidget::pickFontOpacity_il()
{
	LabelStyle* lb = n->getImg();
	lb->setOpacity(ui.il_opacity->value());

	QPalette pal = ui.il_prev->palette();
	color = pal.color(ui.il_prev->foregroundRole());

	color.setAlpha(ui.il_opacity->value());
	pal.setColor(ui.il_prev->foregroundRole(), color);
	ui.il_prev->setPalette(pal);
	float val = float(ui.il_opacity->value()) / float(255) * float(100);
	ui.il_opacityBox->setValue(std::round(val));

	n->setImg(lb);
}

void StyleSettingsWidget::pickBGOpacity_il()
{
	LabelStyle* lb = n->getImg();
	lb->setOpacity_BG(ui.il_opacity_bod->value());

	QPalette pal = ui.il_prev->palette();

	QColor color = pal.color(ui.il_prev->backgroundRole());
	color.setAlpha(ui.il_opacity_bod->value());
	pal.setColor(ui.il_prev->backgroundRole(), color);
	ui.il_prev->setPalette(pal);
	float val = float(ui.il_opacity_bod->value()) / float(255) * float(100);
	ui.il_opacityBox_bod->setValue(std::round(val));

	n->setImg(lb);
}

void StyleSettingsWidget::pickFontOpacityBox_il()
{
	LabelStyle* lb = n->getImg();
	float val = float(ui.il_opacityBox->value()) / float(100) * float(255);
	lb->setOpacity(std::round(val));

	QPalette pal = ui.il_prev->palette();
	color = pal.color(ui.il_prev->foregroundRole());
	color.setAlpha(std::round(val));
	pal.setColor(ui.il_prev->foregroundRole(), color);
	ui.il_prev->setPalette(pal);

	ui.il_opacity->setValue(std::round(val));

	n->setImg(lb);
}


void StyleSettingsWidget::pickBGOpacityBox_il()
{
	LabelStyle* lb = n->getImg();
	float val = float(ui.il_opacityBox_bod->value()) / float(100) * float(255);
	lb->setOpacity_BG(std::round(val));

	QPalette pal = ui.il_prev->palette();
	color = pal.color(ui.il_prev->backgroundRole());
	color.setAlpha(std::round(val));
	pal.setColor(ui.il_prev->backgroundRole(), color);
	ui.il_prev->setPalette(pal);

	ui.il_opacity_bod->setValue(std::round(val));

	n->setImg(lb);
}

void StyleSettingsWidget::pickFontSize_il()
{
	QStringList fontSizes;
	fontSizes << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28" << "36" << "48" << "72";

	LabelStyle* lb = n->getImg();
	lb->setSize(fontSizes.at(ui.il_sizeBox->currentIndex()).toInt());
	const QFont temp = ui.il_prev->font();
	QFont temp2 = temp;
	temp2.setPointSize(fontSizes.at(ui.il_sizeBox->currentIndex()).toInt());
	ui.il_prev->setFont(temp2);

	n->setImg(lb);
}

void StyleSettingsWidget::pickFontColor_il()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.il_colorPicker->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));

	QPalette pal = ui.il_prev->palette();
	pal.setColor(ui.il_prev->foregroundRole(), qRgb(color.red(), color.green(), color.blue()));
	ui.il_prev->setPalette(pal);

	LabelStyle* lb = n->getImg();
	lb->setRed(color.red());
	lb->setBlue(color.blue());
	lb->setGreen(color.green());

	n->setImg(lb);

	pickFontOpacity_il();
}

void StyleSettingsWidget::pickBGColor_il()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	bgColor = dlg.selectedColor();
	ui.il_colorPicker_bod->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(bgColor.red()) + "," + std::to_string(bgColor.green()) + "," + std::to_string(bgColor.blue()) + ");"));

	QPalette pal = ui.il_prev->palette();
	pal.setColor(ui.il_prev->backgroundRole(), qRgb(bgColor.red(), bgColor.green(), bgColor.blue()));
	ui.il_prev->setPalette(pal);

	LabelStyle* lb = n->getImg();
	lb->setRed_BG(bgColor.red());
	lb->setBlue_BG(bgColor.blue());
	lb->setGreen_BG(bgColor.green());

	n->setImg(lb);

	pickBGOpacity_il();
}

void StyleSettingsWidget::pickFont_il()
{
	LabelStyle* lb = n->getImg();
	lb->setFont(ui.il_fontBox->currentFont().family().toStdString());
	const QFont temp = ui.il_prev->font();
	QFont temp2 = temp;
	temp2.setFamily(ui.il_fontBox->currentFont().family());
	ui.il_prev->setFont(temp2);

	n->setImg(lb);
}

void StyleSettingsWidget::pickHeight_il()
{
	LabelStyle* lb = n->getImg();
	lb->setHeight(ui.il_height->value());
	ui.il_prev->setFixedHeight(lb->getHeight());
	n->setImg(lb);
}

void StyleSettingsWidget::pickWidth_il()
{
	LabelStyle* lb = n->getImg();
	lb->setWidth(ui.il_width->value());
	ui.il_prev->setFixedWidth(lb->getWidth());
	n->setImg(lb);
}

void StyleSettingsWidget::pickJustification_il()
{
	LabelStyle* lb = n->getImg();
	QStringList justifications;
	justifications << "Left" << "Center" << "Right" << "Justify";

	lb->setAlign(justifications.at(ui.il_justify->currentIndex()).toStdString());
	if (lb->getAlign() == "Left")
		ui.il_prev->setAlignment(Qt::AlignLeft);
	else if (lb->getAlign() == "Right")
		ui.il_prev->setAlignment(Qt::AlignRight);
	else if (lb->getAlign() == "Justify")
		ui.il_prev->setAlignment(Qt::AlignJustify);
	else
		ui.il_prev->setAlignment(Qt::AlignHCenter);

	n->setImg(lb);
}

void StyleSettingsWidget::pickStyle_il()
{
	LabelStyle* lb = n->getImg();
	QStringList styles;
	styles << "Regular" << "Bold" << "Italic";
	lb->setWeight(styles.at(ui.il_styleBox->currentIndex()).toStdString());

	const QFont temp = ui.il_prev->font();
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

	ui.il_prev->setFont(temp2);
	n->setImg(lb);
}

void StyleSettingsWidget::pickMargin_il()
{
	LabelStyle* lb = n->getImg();
	lb->setMargin(ui.il_margin->value());
	ui.il_prev->setMargin(lb->getMargin());
	n->setImg(lb);
}
#endif // StyleSettingsWidget
