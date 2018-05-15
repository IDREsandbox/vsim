#ifndef SIMPLECOMMANDSTACK_H
#define SIMPLECOMMANDSTACK_H

#include "ICommandStack.h"
#include <QUndoStack>

class SimpleCommandStack : public QObject, public ICommandStack {
public:
    SimpleCommandStack(QObject *parent = nullptr);
    void push(QUndoCommand *cmd) override;
    QUndoStack *stack() const override;
private:
    QUndoStack *m_stack;
};

#endif