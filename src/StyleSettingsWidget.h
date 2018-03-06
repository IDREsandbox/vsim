#ifndef STYLESETTINGSWIDGET_H
#define STYLESETTINGSWIDGET_H
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_StyleSettingsWidget.h"

class LabelStyle;

class StyleSettingsWidget : public QDialog {
	Q_OBJECT
public:
	StyleSettingsWidget(QWidget *parent = nullptr);
	
	void setStyle(const LabelStyle *style);

#ifdef STYLE_SETTINGS
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

#endif // STYLE_SETTINGS


private:
	Ui::StyleSettingsWidget ui;
	QColor m_color;
	QColor m_bg_color;

	QList<int> m_font_sizes = QFontDatabase::standardSizes();
};

#endif