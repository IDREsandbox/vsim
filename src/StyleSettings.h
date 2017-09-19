#pragma once
#ifndef STYLESETTINGS_HPP
#define STYLESETTINGS_HPP
#include <QDialog>
#include <QDialogButtonBox>
#include "Narrative2.h"
#include "LabelStyle.h"

#include "ui_StyleSettings.h"

class StyleSettings : public QDialog {
	Q_OBJECT
public:
	StyleSettings(QWidget *parent = nullptr);
	StyleSettings(Narrative2* narr, QWidget *parent = nullptr);
	~StyleSettings();

	public slots:
	void pickFontColor_h1();
	void pickFontSize_h1();
	void pickFont_h1();
	void pickFontOpacity_h1();
	void pickBGColor_h1();
	void pickBGOpacity_h1();
	//void pickShadow_h1();
	void pickWidth_h1();
	void pickHeight_h1();
	void pickFontOpacityBox_h1();
	void pickBGOpacityBox_h1();
	void pickJustification_h1();
	void pickStyle_h1();
	void pickMargin_h1();


	void pickFontColor_h2();
	void pickFontSize_h2();
	void pickFont_h2();
	void pickFontOpacity_h2();
	void pickBGColor_h2();
	void pickBGOpacity_h2();
	//void pickShadow_h2();
	void pickWidth_h2();
	void pickHeight_h2();
	void pickFontOpacityBox_h2();
	void pickBGOpacityBox_h2();
	void pickJustification_h2();
	void pickStyle_h2();
	void pickMargin_h2();

	void pickFontColor_bt();
	void pickFontSize_bt();
	void pickFont_bt();
	void pickFontOpacity_bt();
	void pickBGColor_bt();
	void pickBGOpacity_bt();
	//void pickShadow_bt();
	void pickWidth_bt();
	void pickHeight_bt();
	void pickFontOpacityBox_bt();
	void pickBGOpacityBox_bt();
	void pickJustification_bt();
	void pickStyle_bt();
	void pickMargin_bt();

	void pickFontColor_lt();
	void pickFontSize_lt();
	void pickFont_lt();
	void pickFontOpacity_lt();
	void pickBGColor_lt();
	void pickBGOpacity_lt();
	//void pickShadow_lt();
	void pickWidth_lt();
	void pickHeight_lt();
	void pickFontOpacityBox_lt();
	void pickBGOpacityBox_lt();
	void pickJustification_lt();
	void pickStyle_lt();
	void pickMargin_lt();

	void pickFontColor_il();
	void pickFontSize_il();
	void pickFont_il();
	void pickFontOpacity_il();
	void pickBGColor_il();
	void pickBGOpacity_il();
	//void pickShadow_il();
	void pickWidth_il();
	void pickHeight_il();
	void pickFontOpacityBox_il();
	void pickBGOpacityBox_il();
	void pickJustification_il();
	void pickStyle_il();
	void pickMargin_il();

private:
	void setGui();
	QColor color;
	QColor bgColor;
	Ui::Dialog ui;
	Narrative2* n;
};

#endif // STYLESETTINGS_HPP