#ifndef ERESOURCEGROUP_H
#define ERESOURCEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"
#include "EResource.h"

class EResourceGroup : public Group {
	Q_OBJECT

public:
	EResourceGroup() {}
	EResourceGroup(const EResourceGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	META_Node(, EResourceGroup)

signals :
	void sNewEResource(int);
	void sDeleteEResource(int);

public: // COMMANDS
	class NewEResourceCommand : public Group::NewNodeCommand<EResource> {
	public:
		NewEResourceCommand(EResourceGroup *group, int index, QUndoCommand *parent = nullptr)
			: Group::NewNodeCommand<EResource>(group, index, parent) {}
	};
	class DeleteEResourceCommand : public Group::DeleteNodeCommand<EResource> {
	public:
		DeleteEResourceCommand(EResourceGroup *group, int index, QUndoCommand *parent = nullptr)
			: Group::DeleteNodeCommand<EResource>(group, index, parent) {}
	};
};

#endif