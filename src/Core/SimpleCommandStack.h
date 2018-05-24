#ifndef SIMPLECOMMANDSTACK_H
#define SIMPLECOMMANDSTACK_H

#include "ICommandStack.h"
#include <QUndoStack>

class SimpleCommandStack : public QObject, public ICommandStack {
public:
    SimpleCommandStack(QObject *parent = nullptr);
    //void push(QUndoCommand *cmd) override;

	QUndoCommand *begin() override;
	void end() override;

    QUndoStack *stack() const;
private:
    QUndoStack *m_stack;
	QUndoCommand *m_cmd;
};

#endif