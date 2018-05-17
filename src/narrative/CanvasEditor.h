#ifndef CANVASEDITOR_H
#define CANVASEDITOR_H

#include <QWidget>
#include <QMainWindow>
#include <QAbstractButton>
#include <memory>
#include <vector>

#include "LabelType.h"

class CanvasScene;
class CanvasControl;
class CanvasToolBar;
class CanvasContainer;
class ICommandStack;
class LabelStyleGroup;

// Canvas editing widget
// - owns a CanvasControl for modifying a CanvasScene
// - owns and connects an editing toolbar to the CanvasControl
// - owns a CanvasContainer for viewing a CanvasScene
class CanvasEditor : public QWidget {
	Q_OBJECT;
public:
	CanvasEditor(QWidget *parent = nullptr);

	void setScene(CanvasScene *scene);
	void setStack(ICommandStack *stack);
	void setStyles(LabelStyleGroup *styles);
	void applyStylesToButtons();

	void showToolBar(bool show);

	CanvasControl *control();
	CanvasContainer *container();
	QMainWindow *internalWindow();

	void debug();

signals:
	void sDone();
	void sEditStyles();

private:
	void updateToolBar();

private:
	// gui
	CanvasContainer *m_container;
	CanvasToolBar *m_tb;

	CanvasControl *m_cc;

	QMainWindow *m_internal_window;

	LabelStyleGroup *m_styles;
	std::unique_ptr<LabelStyleGroup> m_default_styles;

	std::vector<std::pair<QAbstractButton*, LabelType>>
		m_button_type_map;
};

#endif
