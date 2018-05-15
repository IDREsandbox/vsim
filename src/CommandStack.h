#ifndef COMMANDSTACK_H
#define COMMANDSTACK_H

#include <QUndoStack>

class ICommandStack {
public:
	virtual push(QUndoCommand *);
};

#endif