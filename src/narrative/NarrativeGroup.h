#ifndef NARRATIVEGROUP_H
#define NARRATIVEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"
#include "Narrative2.h"

class NarrativeGroup : public Group {
	Q_OBJECT
public:
	NarrativeGroup() {}
	NarrativeGroup(const NarrativeGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	NarrativeGroup(osg::Group *old_group);
	META_Node(, NarrativeGroup)

public: // COMMANDS
	class NewNarrativeCommand : public Group::NewNodeCommand<Narrative2> {
	public:
		NewNarrativeCommand(NarrativeGroup *group, int index, QUndoCommand *parent = nullptr)
			: Group::NewNodeCommand<Narrative2>(group, index, parent) {}
	};
	class DeleteNarrativeCommand : public Group::DeleteNodeCommand<Narrative2> {
	public:
		DeleteNarrativeCommand(NarrativeGroup *group, int index, QUndoCommand *parent = nullptr)
			: Group::DeleteNodeCommand<Narrative2>(group, index, parent) {}
	};

	// add a narrative to the end, for importing
	class AddNarrativeCommand : public QUndoCommand {
	public:
		AddNarrativeCommand(NarrativeGroup *group, Narrative2 *narrative, QUndoCommand *parent = nullptr);
		void undo();
		void redo();
	private:
		NarrativeGroup *m_group;
		osg::ref_ptr<Narrative2> m_narrative;
	};
};

#endif