#ifndef STYLESETTINGSWIDGET_H
#define STYLESETTINGSWIDGET_H
#include <QDialog>
#include <QDialogButtonBox>
#include <memory>

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

	NarrativeCanvas *m_canvas; // qt owned
	std::shared_ptr<NarrativeSlide> m_slide;
	std::shared_ptr<NarrativeSlideLabel> m_label;

	std::unique_ptr<LabelStyle> m_style;

	QList<int> m_font_sizes = QFontDatabase::standardSizes();
};

#endif