#ifndef STYLESETTINGSWIDGET_H
#define STYLESETTINGSWIDGET_H
#include <QDialog>
#include <QDialogButtonBox>

#include "ui_StyleSettingsWidget.h"

class LabelStyle;
class NarrativeCanvas;
class NarrativeSlide;
class NarrativeSlideLabel;

class StyleSettingsWidget : public QDialog {
	Q_OBJECT
public:
	StyleSettingsWidget(QWidget *parent = nullptr);

	void setStyle(const LabelStyle *style);
	LabelStyle *getStyle() const;

private:
	void refresh();

private:
	Ui::StyleSettingsWidget ui;

	NarrativeCanvas *m_canvas;
	NarrativeSlide *m_slide;
	NarrativeSlideLabel *m_label;

	LabelStyle *m_style;

	QList<int> m_font_sizes = QFontDatabase::standardSizes();
};

#endif