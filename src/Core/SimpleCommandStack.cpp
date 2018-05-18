#include "SimpleCommandStack.h"

SimpleCommandStack::SimpleCommandStack(QObject *parent)
    : QObject(parent) {
    m_stack = new QUndoStack(this);
}

void SimpleCommandStack::push(QUndoCommand *cmd) {
    m_stack->push(cmd);
}

QUndoStack *SimpleCommandStack::stack() const {
    return m_stack;
}
