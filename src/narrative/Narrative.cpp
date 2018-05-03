#include <assert.h>
#include "narrative/Narrative.h"
#include "narrative/NarrativeSlide.h"
#include "deprecated/narrative/NarrativeNode.h"
#include "deprecated/narrative/NarrativeOld.h"
#include "narrative/NarrativeSlide.h"
#include "LabelStyleGroup.h"
#include "WeakObject.h"

Narrative::Narrative(QObject *parent)
	: TGroup<NarrativeSlide>(parent),
	m_title("Untitled"),
	m_description(""),
	m_author(""),
	m_locked(false)
{
	m_styles = new LabelStyleGroup(this);
}

void Narrative::loadOld(const NarrativeOld * old)
{
	m_title = old->getName();
	m_description = old->getDescription();
	m_author = old->getAuthor();
	m_locked = old->getLock();
	clear();
	for (uint i = 0; i < old->getNumNodes(); i++) {
		NarrativeNode *node = old->getNode(i);
		NarrativeTransition *transition = old->getTransition(i);
		auto new_slide = std::make_shared<NarrativeSlide>();
		new_slide->loadOld(node, transition);
		append(new_slide);
	}
}

const std::string& Narrative::getTitle() const{
	return m_title;
}
void Narrative::setTitle(const std::string & title)
{
	emit sTitleChanged(title);
	m_title = title;
}
const std::string& Narrative::getAuthor() const {
	return m_author;
}
void Narrative::setAuthor(const std::string& author) {
	emit sAuthorChanged(author);
	m_author = author;
}
const std::string& Narrative::getDescription() const {
	return m_description;
}
void Narrative::setDescription(const std::string& description) {
	emit sDescriptionChanged(description);
	m_description = description;
}
LabelStyleGroup * Narrative::labelStyles() const
{
	return m_styles;
}
