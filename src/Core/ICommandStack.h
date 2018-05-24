#ifndef ICOMMANDSTACK_H
#define ICOMMANDSTACK_H

#include <QUndoStack>

// For wrapping a QUndoStack if you need to add context
// before/after a command push

// example:
// SelectSlideCommand (NarrativeControl)
//   SelectLabelCommand (CanvasControl)
//   EditTextCommand (CanvasControl)
// Canvas PoV:
//   auto *cmd = m_stack->begin()
//   new SelectLabelCommand({label}, cmd);
//   m_stack->end()
// This allows CanvasControl to know nothing about Narratives.

// QUndoCommand doesn't have setParent() or addChild() which means
// we can't do ICommandStack::push(QUndoCommand*) because the parent
// will be fixed as null. -> we have to inject a parent somehow.

class ICommandStack {
public:
	//virtual void push(QUndoCommand *cmd) = 0;
	virtual QUndoCommand *begin() {
		return nullptr;
	}
	virtual void end() {
	}

	//virtual QUndoStack *stack() {
	//	return nullptr;
	//}
};

#endif