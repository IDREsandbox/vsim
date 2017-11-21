#include "NarrativeGroup.h"

NarrativeGroup::NarrativeGroup(const osg::Group *old_group) 
{
	for (int i = old_group->getNumChildren() - 1; i >= 0; i--) {
		const osg::Node *node = old_group->getChild(i);
		const Narrative *old_narrative = dynamic_cast<const Narrative*>(node);
		if (old_narrative) {
			qDebug() << "Found an old narrative" << QString::fromStdString(old_narrative->getName()) << "- converting";
			Narrative2 *new_narrative = new Narrative2(old_narrative);
			this->addChild(new_narrative);
		}
	}
}

NarrativeGroup::AddNarrativeCommand::AddNarrativeCommand(NarrativeGroup * group, Narrative2 * narrative, QUndoCommand * parent)
	: QUndoCommand(parent),
	m_group(group),
	m_narrative(narrative)
{
}

void NarrativeGroup::AddNarrativeCommand::undo()
{
	uint index = m_group->getNumChildren() - 1;
	m_group->removeChild(index);
	m_group->sDelete(index);
}

void NarrativeGroup::AddNarrativeCommand::redo()
{
	uint index = m_group->getNumChildren();
	m_group->addChild(m_narrative);
	m_group->sNew(index);
}
