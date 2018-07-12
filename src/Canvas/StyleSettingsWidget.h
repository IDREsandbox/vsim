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
class CanvasImage;
class FrameStyle;

// Edits a LabelStyle in LABEL mode (default) or a FrameStyle in IMAGE mode.
class StyleSettingsWidget : public QDialog {
	Q_OBJECT
public:
	StyleSettingsWidget(QWidget *parent = nullptr);

	enum class Mode {
		LABEL,
		IMAGE
	};
	void setMode(Mode mode);

	// edit a label style (for labels)
	void setStyle(const LabelStyle *style);
	LabelStyle *getStyle() const;

	// edit a frame style (for images)
	void setFrameStyle(const FrameStyle *fstyle);
	const FrameStyle *getFrameStyle() const;

private:
	void refresh();

	void setFrameStyleInternal(const FrameStyle *style);

private:
	Ui::StyleSettingsWidget ui;
	Mode m_mode;

	CanvasScene *m_scene; // qt owned
	CanvasContainer *m_canvas;
	CanvasControl *m_control;
	std::shared_ptr<CanvasLabel> m_label;
	std::shared_ptr<CanvasImage> m_image;

	std::unique_ptr<LabelStyle> m_style;

	QList<int> m_font_sizes = QFontDatabase::standardSizes();
};

#endif