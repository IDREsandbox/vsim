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
	//void pickShadow_h1();
	void pickBGColor_h1();
	void pickBGOpacity_h1();
	//void pickWidth_h1();
	//void pickHeight_h1();

private:
	void setGui();
	QColor color;
	QColor bgColor;
	Ui::Dialog ui;
	Narrative2* n;
};

#endif // STYLESETTINGS_HPP