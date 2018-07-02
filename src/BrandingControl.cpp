#include "BrandingControl.h"

#include <QAction>

#include "BrandingOverlay.h"
#include "Canvas/CanvasEditor.h"
#include "VSimApp.h"

BrandingControl::BrandingControl(VSimApp *app,
	CanvasScene *scene,
	BrandingOverlay *overlay,
	QObject * parent)
	: QObject(parent),
	m_app(app),
	m_overlay(overlay),
	m_canvas_stack_wrapper(this)
{
	CanvasEditor *editor = m_overlay->m_editor;
	editor->setStack(&m_canvas_stack_wrapper);
	editor->setScene(scene);

	a_edit = new QAction(this);
	a_edit->setText("Edit Branding");
	a_edit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));

	m_overlay->enableEditing(false);

	connect(app, &VSimApp::sStateChanged, this, [this](VSimApp::State state) {
		if (state == VSimApp::State::EDIT_BRANDING) {
			onActivate();
		}
		else {
			onDeactivate();
		}
	});

	connect(editor, &CanvasEditor::sDone, this, &BrandingControl::deactivate);
	connect(a_edit, &QAction::triggered, this, [this]() {
		if (m_app->state() == VSimApp::State::EDIT_BRANDING) {
			deactivate();
		}
		else {
			activate();
		}
	});
}

void BrandingControl::activate()
{
	m_app->setState(VSimApp::State::EDIT_BRANDING);
	m_overlay->enableEditing(true);
}

void BrandingControl::deactivate()
{
	m_app->setState(VSimApp::State::EDIT_FLYING);
	m_overlay->enableEditing(false);
}

void BrandingControl::onActivate()
{
	m_overlay->enableEditing(true);
}

void BrandingControl::onDeactivate()
{
	m_overlay->enableEditing(false);
}

BrandingControl::CanvasStackWrapper::CanvasStackWrapper(BrandingControl *control)
	: m_control(control)
{
}

QUndoCommand *BrandingControl::CanvasStackWrapper::begin()
{
	m_cmd = new BrandingCommand(m_control);
	return m_cmd;
}

void BrandingControl::CanvasStackWrapper::end()
{
	m_control->m_app->getUndoStack()->push(m_cmd);
}

BrandingControl::BrandingCommand::BrandingCommand(BrandingControl *control)
	: m_control(control)
{
}

void BrandingControl::BrandingCommand::undo()
{
	activate();
	QUndoCommand::undo();
}

void BrandingControl::BrandingCommand::redo()
{
	activate();
	QUndoCommand::redo();
}

void BrandingControl::BrandingCommand::activate()
{
	m_control->activate();
}
