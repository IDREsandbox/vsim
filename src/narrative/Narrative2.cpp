#include <assert.h>
#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"
#include "deprecated/narrative/NarrativeNode.h"
#include "LabelStyleGroup.h"

Narrative2::Narrative2()
	: Group(),
	m_title("Untitled"),
	m_description(""),
	m_author(""),
	m_locked(false)
{
	setLabelStyles(new LabelStyleGroup());
}

Narrative2::Narrative2(const Narrative2 & n, const osg::CopyOp & copyop)
	: Group(n, copyop)
{
}

Narrative2::Narrative2(const Narrative * old)
	: Narrative2()
{
	m_title = old->getName();
	m_description = old->getDescription();
	m_author = old->getAuthor();
	m_locked = old->getLock();
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
	emit sTitleChanged(title);
	m_title = title;
}
const std::string& Narrative2::getAuthor() const {
	return m_author;
}
void Narrative2::setAuthor(const std::string& author) {
	emit sAuthorChanged(author);
	m_author = author;
}
const std::string& Narrative2::getDescription() const {
	return m_description;
}
void Narrative2::setDescription(const std::string& description) {
	emit sDescriptionChanged(description);
	m_description = description;
}
LabelStyleGroup * Narrative2::labelStyles() const
{
	return m_styles;
}
const LabelStyleGroup * Narrative2::getLabelStyles() const
{
	return m_styles;
}
void Narrative2::setLabelStyles(LabelStyleGroup *styles)
{
	if (styles == nullptr) styles = new LabelStyleGroup();
	m_styles = styles;
	m_styles->init();
}