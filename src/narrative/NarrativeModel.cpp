#include "narrative/NarrativeModel.h"
#include "Util.h"
#include "RegisterMetaTypes.h"


NarrativeModel::NarrativeModel(osg::Group *root, QUndoStack *stack, QObject *parent)
	: m_root(root),
	m_stack(stack)
{
}

class NarrativeCommand : public QUndoCommand {
public:
	NarrativeCommand(NarrativeModel *model, QUndoCommand *parent = nullptr)
		: QUndoCommand(parent) {}
protected:
	NarrativeModel *m_model;
};

// partially owns the narrative
class NewNarrativeCommand : public NarrativeCommand {
private:
	osg::ref_ptr<Narrative2> m_narrative;
	int m_index;
public:
	NewNarrativeCommand(int index, NarrativeModel *model, QUndoCommand *parent = nullptr)
		: NarrativeCommand(model, parent),
		m_index(index)
	{
		m_narrative = new Narrative2;
	}
	void undo() {
		m_model->getRoot()->removeChild(m_narrative);
		m_model->sDeleteNarrative(m_index);
	}
	void redo() {
		m_model->getRoot()->insertChild(m_index, m_narrative);
		m_model->sNewNarrative(m_index);
	}
};

// partially owns the narrative
class DeleteNarrativeCommand : public NarrativeCommand {
private:
	osg::ref_ptr<Narrative2> m_narrative;
	int m_index;
public:
	DeleteNarrativeCommand(int index, NarrativeModel *model, QUndoCommand *parent = nullptr)
		: NarrativeCommand(model, parent),
		m_index(index)
	{
		m_narrative = model->getNarrative(index);
	}
	void undo() {
		m_model->getNarrative(m_index)->insertChild(m_index, m_narrative);
		m_model->sNewNarrative(m_index);
	}
	void redo() {
		m_model->getNarrative(m_index)->removeChild(m_narrative);
		m_model->sDeleteNarrative(m_index);
	}
};
class SetNarrativeInfoCommand : public QUndoCommand {
private:
	NarrativeModel *m_model;

	Narrative2 *m_narrative;
	std::string old_title;
	std::string old_description;
	std::string old_author;
	std::string new_title;
	std::string new_description;
	std::string new_author;

public:
	SetNarrativeInfoCommand(NarrativeModel *model, 
		Narrative2 *narrative,
		const std::string &title,
		const std::string &description,
		const std::string &author,
		QUndoCommand *parent = nullptr)

		: QUndoCommand(parent),
		m_model(model),
		m_narrative(narrative),
		old_title(narrative->getTitle()),
		old_description(narrative->getDescription()),
		old_author(narrative->getAuthor()),
		new_title(title),
		new_description(description),
		new_author(author)
	{
	}
	void undo() {
		m_narrative->setTitle(old_title);
		m_narrative->setDescription(old_description);
		m_narrative->setAuthor(old_author);
	}
	void redo() {
		m_narrative->setTitle(new_title);
		m_narrative->setDescription(new_description);
		m_narrative->setAuthor(new_author);
	}
};

//class SetNarrativeInfoCommand;
class MoveNarrativeCommand : public QUndoCommand {
private:
	osg::ref_ptr<Narrative2> m_narrative;
	osg::Group *m_group;
	int m_old_index;
	int m_new_index;

public:
	MoveNarrativeCommand(osg::Group *group, int new_index, int old_index, QUndoCommand *parent = nullptr)
		: m_group(group),
		m_old_index(old_index),
		m_new_index(new_index)
	{
		osg::Node *node = group->getChild(old_index);
		m_narrative = dynamic_cast<Narrative2*>(node);
	}
	void undo() {
		m_group->removeChild(m_narrative);
		m_group->insertChild(m_old_index, m_narrative);
	}
	void redo() {
		m_group->removeChild(m_narrative);
		m_group->insertChild(m_new_index, m_narrative);
	}
};

//class DuplicateNarrativeCommand;
//class ImportNarrativeCommand;

//class NewSlideCommand;
//
//class NewSlideCommand : public QUndoStack {
//private:
//	Narrative2 *m_narrative; // parent
//	int m_index;
//
//public:
//NewSlideCommand(Narrative2 *narrative, m_index);

Narrative2 * NarrativeModel::getNarrative(int narrative)
{
	if (narrative >= m_root->getNumChildren()) return nullptr;
	osg::Node *node = m_root->getChild(narrative);
	return dynamic_cast<Narrative2*>(node);
}

NarrativeSlide * NarrativeModel::getSlide(int narrative, int slide)
{
	Narrative2 *parent = getNarrative(narrative);
	if (!parent) return nullptr;
	if (slide >= parent->getNumChildren()) return nullptr;
	osg::Node *node = parent->getChild(slide);
	return dynamic_cast<NarrativeSlide*>(node);
}

NarrativeSlideLabels * NarrativeModel::getLabel(int narrative, int slide, int label)
{
	NarrativeSlide *parent = getSlide(narrative, slide);
	if (!parent) return nullptr;
	if (label >= parent->getNumChildren()) return nullptr;
	osg::Node *node = parent->getChild(slide);
	return dynamic_cast<NarrativeSlideLabels*>(node);
}

void NarrativeModel::newNarrative(int index)
{
	//m_stack->push(new NewNarrativeCommand(this, m_root, index));
}

void NarrativeModel::deleteNarrative(int index)
{
}

void NarrativeModel::setNarrativeInfo()
{
}

void NarrativeModel::moveNarrative()
{
}
