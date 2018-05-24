#ifndef CANVASEDITOR_H
#define CANVASEDITOR_H

#include <QWidget>
#include <QMainWindow>
#include <QAbstractButton>
#include <memory>
#include <vector>

#include "Canvas/LabelType.h"

class CanvasScene;
class CanvasControl;
class CanvasToolBar;
class CanvasContainer;
class ICommandStack;
class LabelStyleGroup;
class CanvasWindow;

// Canvas editing widget
// - owns a CanvasControl for modifying a CanvasScene
// - owns and connects an editing toolbar to the CanvasControl
// - owns a CanvasContainer for viewing a CanvasScene
class CanvasEditor : public QWidget {
	Q_OBJECT;
public:
	CanvasEditor(QWidget *parent = nullptr);

	CanvasScene *scene() const;
	void setScene(CanvasScene *scene);
	void setStack(ICommandStack *stack);
	void setStyles(LabelStyleGroup *styles);
	void applyStylesToButtons();
	void setEditable(bool editable);
	bool isEditable() const;

	CanvasControl *control();
	CanvasContainer *container();
	CanvasWindow *internalWindow();

	void debug();

public: //actions
	QAction *a_delete;

signals:
	void sDone();
	void sEditStyles();

protected:
	void hideEvent(QHideEvent *e) override;
	void showEvent(QShowEvent *e) override;

private:
	void updateToolBar();

private:
	// gui
	CanvasContainer *m_container;
	CanvasToolBar *m_tb;

	CanvasControl *m_cc;
	CanvasScene *m_scene;

	CanvasWindow *m_internal_window;

	LabelStyleGroup *m_styles;
	std::unique_ptr<LabelStyleGroup> m_default_styles;

	std::vector<std::pair<QAbstractButton*, LabelType>>
		m_button_type_map;

	QString m_last_image_dir;
	bool m_editing_enabled;
};


class CanvasWindow : public QMainWindow {
public:
	CanvasWindow(QWidget *parent = nullptr);

	void calcMask();

	bool event(QEvent *e) override;
protected:
	void resizeEvent(QResizeEvent *e) override;

signals:
	//void sMaskChanged(const QRegion &r);
};

#endif
