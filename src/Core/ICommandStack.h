#ifndef ICOMMANDSTACK_H
#define ICOMMANDSTACK_H

#include <QUndoStack>

// For wrapping and injecting a QUndoStack
// If you need to add context before/after a command push
// ex: We want to write the canvas independently of narratives
//     but push SelectNarrative() on every canvas command
class ICommandStack {
public:
	virtual void push(QUndoCommand *cmd) = 0;
    virtual QUndoStack *stack() const = 0;
};

#endif