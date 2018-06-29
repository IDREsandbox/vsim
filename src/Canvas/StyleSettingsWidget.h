#ifndef STYLESETTINGSWIDGET_H
#define STYLESETTINGSWIDGET_H
#include <QDialog>
#include <QDialogButtonBox>
#include <memory>

#include "ui_StyleSettingsWidget.h"

class LabelStyle;

class CanvasScene;
class CanvasContainer;
class CanvasControl;
class CanvasLabel;

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

	CanvasScene *m_scene; // qt owned
	CanvasContainer *m_canvas;
	CanvasControl *m_control;
	std::shared_ptr<CanvasLabel> m_label;

	std::unique_ptr<LabelStyle> m_style;

	QList<int> m_font_sizes = QFontDatabase::standardSizes();
};

#endif