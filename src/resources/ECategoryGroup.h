#pragma once
#ifndef ECATEGORYGROUP_H
#define ECATEGORYGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"
#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategory.h"

class ECategoryGroup : public Group {
	Q_OBJECT

public:
	ECategoryGroup() {}
	ECategoryGroup(const ECategoryGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	ECategoryGroup(osg::Group *group); // conversion, pass in something that hold EResources
	META_Node(, ECategoryGroup)

signals :
	void sNewECategory(int);
	void sDeleteECategory(int);

public: // COMMANDS
	class NewECategoryCommand : public Group::NewNodeCommand<ECategory> {
	public:
		NewECategoryCommand(ECategoryGroup *group, int index, QUndoCommand *parent = nullptr)
			: Group::NewNodeCommand<ECategory>(group, index, parent) {}
	};
	class DeleteECategoryCommand : public Group::DeleteNodeCommand<ECategory> {
	public:
		DeleteECategoryCommand(ECategoryGroup *group, int index, QUndoCommand *parent = nullptr)
			: Group::DeleteNodeCommand<ECategory>(group, index, parent) {}
	};
	class AddECategoryCommand : public QUndoCommand {
	public:
		AddECategoryCommand(ECategoryGroup *group, ECategory *ECat, QUndoCommand *parent = nullptr);
		void undo();
		void redo();
	private:
		ECategoryGroup *m_group;
		osg::ref_ptr<ECategory> m_category;
	};
};

#endif