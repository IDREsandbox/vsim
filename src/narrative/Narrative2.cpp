#include <assert.h>
#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"
#include "deprecated/narrative/NarrativeNode.h"

Narrative2::Narrative2()
	: osg::Group(),
	m_title("Untitled"),
	m_description(""),
	m_author(""),
	m_locked(false)
{
}

Narrative2::Narrative2(const Narrative2 & n, const osg::CopyOp & copyop)
	: osg::Group(n, copyop),
	m_title(n.m_title),
	m_description(n.m_description),
	m_author(n.m_author),
	m_locked(n.m_locked)
{
}

Narrative2::Narrative2(const Narrative * old)
	: osg::Group(),
	m_title(old->getName()),
	m_description(old->getDescription()),
	m_author(old->getAuthor()),
	m_locked(old->getLock())
{
	for (uint i = 0; i < old->getNumNodes(); i++) {
		NarrativeNode *node = old->getNode(i);
		NarrativeTransition *transition = old->getTransition(i);
		NarrativeSlide *new_slide = new NarrativeSlide(node, transition);
		addChild(new_slide);
	}
}

Narrative2::~Narrative2()
{
}

const std::string& Narrative2::getTitle() const{
	return m_title;
}
void Narrative2::setTitle(const std::string & title)
{
	m_title = title;
}
const std::string& Narrative2::getAuthor() const {
	return m_author;
}
void Narrative2::setAuthor(const std::string& author) {
	m_author = author;
}
const std::string& Narrative2::getDescription() const {
	return m_description;
}
void Narrative2::setDescription(const std::string& description) {
	m_description = description;
}

Narrative2::NewSlideCommand::NewSlideCommand(Narrative2 *narrative, int slide_index, QUndoCommand * parent)
	: QUndoCommand(parent),
	m_narrative(narrative),
	m_index(slide_index)
{
	m_slide = new NarrativeSlide;
}
void Narrative2::NewSlideCommand::undo()
{
	m_narrative->removeChild(m_index);
	m_narrative->sDeleteSlide(m_index);
}
void Narrative2::NewSlideCommand::redo()
{
	m_narrative->insertChild(m_index, m_slide);
	m_narrative->sNewSlide(m_index);
}

Narrative2::DeleteSlideCommand::DeleteSlideCommand(Narrative2 *narrative, int slide_index, QUndoCommand * parent)
	: QUndoCommand(parent),
	m_narrative(narrative),
	m_index(slide_index)
{
	NarrativeSlide *slide = dynamic_cast<NarrativeSlide*>(narrative->getChild(slide_index));
	m_slide = slide;
}
void Narrative2::DeleteSlideCommand::undo()
{
	m_narrative->insertChild(m_index, m_slide);
	m_narrative->sNewSlide(m_index);
}
void Narrative2::DeleteSlideCommand::redo()
{
	m_narrative->removeChild(m_index);
	m_narrative->sDeleteSlide(m_index);
}
