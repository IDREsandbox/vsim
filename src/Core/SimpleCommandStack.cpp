#include "SimpleCommandStack.h"

SimpleCommandStack::SimpleCommandStack(QObject *parent)
    : QObject(parent) {
    m_stack = new QUndoStack(this);
}

QUndoCommand *SimpleCommandStack::begin()
{
	m_cmd = new QUndoCommand();
	return m_cmd;
}

void SimpleCommandStack::end()
{
	m_stack->push(m_cmd);
}

QUndoStack * SimpleCommandStack::stack() const
{
	return m_stack;
}
