#ifndef BRANDINGCONTROL_H
#define BRANDINGCONTROL_H

#include <QUndoCommand>

#include "SelectionStack.h"
//#include "Core/Command.h"
#include "Core/ICommandStack.h"
#include "VSimApp.h"

class CanvasEditor;
class BrandingOverlay;
class CanvasScene;

// Bridges osg and gui for narratives
class BrandingControl : public QObject
{
	Q_OBJECT;
public:
	BrandingControl(VSimApp *app,
		CanvasScene *scene,
		BrandingOverlay *overlay,
		QObject *parent = nullptr);

	//void load(CanvasScene *scene);

	void activate();
	void deactivate();

	void onActivate();
	void onDeactivate();
	void onLockChanged();

public: // Actions
	QAction *a_edit;
	QAction *a_end_edit;

private: // Classes

	// just preps the state
	class BrandingCommand : public QUndoCommand {
	public:
		BrandingCommand(BrandingControl *m_app);
		void undo() override;
		void redo() override;
		void activate();
	private:
		BrandingControl * m_control;
	};

	class CanvasStackWrapper : public ICommandStack {
	public:
		CanvasStackWrapper(BrandingControl *control);
		QUndoCommand *begin() override;
		void end() override;
	private:
		BrandingControl * m_control;
		QUndoCommand *m_cmd;
	};

private:
	VSimApp *m_app;

	BrandingOverlay *m_overlay;

	CanvasEditor *m_canvas;

	QUndoStack *m_undo_stack;
	CanvasStackWrapper m_canvas_stack_wrapper;

	bool m_read_only;
};

#endif
