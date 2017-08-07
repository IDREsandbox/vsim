#ifndef NARRATIVEGROUP_H
#define NARRATIVEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Narrative2.h"

class NarrativeGroup : public QObject, public osg::Group {
	Q_OBJECT
public:
	NarrativeGroup();
	NarrativeGroup(const NarrativeGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	META_Node(, NarrativeGroup)

signals:
	void sNewNarrative(int);
	void sDeleteNarrative(int);

public: // COMMANDS
	class NewNarrativeCommand : public NewNodeCommand<NarrativeGroup, Narrative2> {
	public:
		NewNarrativeCommand(NarrativeGroup *group, int index, QUndoCommand *parent = nullptr)
			: NewNodeCommand(&NarrativeGroup::sNewNarrative, &NarrativeGroup::sDeleteNarrative, group, index, parent) {}
	};

	class DeleteNarrativeCommand : public DeleteNodeCommand<NarrativeGroup, Narrative2> {
	public:
		DeleteNarrativeCommand(NarrativeGroup *group, int index, QUndoCommand *parent = nullptr)
			: DeleteNodeCommand(&NarrativeGroup::sNewNarrative, &NarrativeGroup::sDeleteNarrative, group, index, parent) {}
	};


	//class NewNarrativeCommand : public QUndoCommand {
	//public:
	//	NewNarrativeCommand(NarrativeGroup *group, int narrative_index, QUndoCommand *parent = nullptr);
	//	void undo();
	//	void redo();
	//private:
	//	Narrative2 *m_narrative;
	//	osg::ref_ptr<NarrativeSlide> m_slide;
	//	int m_index;
	//};
	//class DeleteSlideCommand : public QUndoCommand {
	//public:
	//	DeleteSlideCommand(Narrative2 *narrative, int slide_index, QUndoCommand *parent = nullptr);
	//	void undo();
	//	void redo();
	//private:
	//	Narrative2 *m_narrative;
	//	osg::ref_ptr<NarrativeSlide> m_slide;
	//	int m_index;
	//};

};

#endif