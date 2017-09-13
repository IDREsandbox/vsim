#pragma once
#ifndef LABELSTYLEGROUP
#define LABELSTYLEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"
#include "LabelStyle.h"

class LabelStyleGroup : public Group {
	Q_OBJECT

public:
	LabelStyleGroup() {}
	LabelStyleGroup(const LabelStyleGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	//LabelStyleGroup(osg::Group *old_group);
	META_Node(, LabelStyleGroup)

signals :
	void sNewLabelStyle(int);
	void sDeleteLabelStyle(int);

public: // COMMANDS
	class NewLabelStyleCommand : public Group::NewNodeCommand<LabelStyle> {
	public:
		NewLabelStyleCommand(LabelStyleGroup *group, int index, QUndoCommand *parent = nullptr)
			: Group::NewNodeCommand<LabelStyle>(group, index, parent) {}
	};
	class DeleteLabelStyleCommand : public Group::DeleteNodeCommand<LabelStyle> {
	public:
		DeleteLabelStyleCommand(LabelStyleGroup *group, int index, QUndoCommand *parent = nullptr)
			: Group::DeleteNodeCommand<LabelStyle>(group, index, parent) {}
	};
	class AddEResourceCommand : public QUndoCommand {
	public:
		AddLabelStyleCommand(LabelStyleGroup *group, LabelStyle *style, QUndoCommand *parent = nullptr);
		void undo();
		void redo();
	private:
		LabelStyleGroup *m_group;
		osg::ref_ptr<LabelStyle> m_style;
	};
};

#endif // LABELSTYLEGROUP_H