#include "narrative/Narrative.h"

#include "narrative/NarrativeSlide.h"
#include "deprecated/narrative/NarrativeNode.h"
#include "deprecated/narrative/NarrativeOld.h"
#include "narrative/NarrativeSlide.h"
#include "Canvas/LabelStyleGroup.h"
#include "Core/LockTable.h"

Narrative::Narrative(QObject *parent)
	: TGroup<NarrativeSlide>(parent),
	m_title("Untitled"),
	m_description(""),
	m_author("")
{
	m_lock_table = new LockTable(this);
	m_styles = std::make_unique<LabelStyleGroup>();
}

Narrative & Narrative::operator=(const Narrative & other)
{
	TGroup<NarrativeSlide>::operator=(other); // copy slides
	m_author = other.m_author;
	m_description = other.m_description;
	m_title = other.m_title;
	*m_lock_table = *other.m_lock_table;
	m_styles->copy(*other.m_styles.get());
	return *this;
}

void Narrative::loadOld(const NarrativeOld * old)
{
	m_title = old->getName();
	m_description = old->getDescription();
	m_author = old->getAuthor();
	// unlocked
	clear();
	for (uint i = 0; i < old->getNumNodes(); i++) {
		NarrativeNode *node = old->getNode(i);
		NarrativeTransition *transition = old->getTransition(i);
		auto new_slide = std::make_shared<NarrativeSlide>();
		new_slide->loadOld(node, transition);
		append(new_slide);
	}
}

std::string Narrative::getTitle() const{
	return m_title;
}
void Narrative::setTitle(const std::string & title)
{
	emit sTitleChanged(title);
	m_title = title;
}
std::string Narrative::getAuthor() const {
	return m_author;
}
void Narrative::setAuthor(const std::string& author) {
	emit sAuthorChanged(author);
	m_author = author;
}
std::string Narrative::getDescription() const {
	return m_description;
}
void Narrative::setDescription(const std::string& description) {
	emit sDescriptionChanged(description);
	m_description = description;
}
LockTable *Narrative::lockTable()
{
	return m_lock_table;
}
const LockTable * Narrative::lockTableConst() const
{
	return m_lock_table;
}
LabelStyleGroup * Narrative::labelStyles() const
{
	return m_styles.get();
}
