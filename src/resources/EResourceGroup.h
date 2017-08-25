#ifndef ERESOURCEGROUP_H
#define ERESOURCEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"
#include "resources/EResource.h"

class EResourceGroup : public Group {
	Q_OBJECT

public:
	EResourceGroup() {}
	EResourceGroup(const EResourceGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	//EResourceGroup(osg::Group *old_group);
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
	class AddEResourceCommand : public QUndoCommand {
	public:
		AddEResourceCommand(EResourceGroup *group, EResource *narrative, QUndoCommand *parent = nullptr);
		void undo();
		void redo();
	private:
		EResourceGroup *m_group;
		osg::ref_ptr<EResource> m_resource;
	};
};

#endif