#include <assert.h>
#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"
#include "deprecated/narrative/NarrativeNode.h"

Narrative2::Narrative2()
	: Group(),
	m_title("Untitled"),
	m_description(""),
	m_author(""),
	m_locked(false)
{
	h1 = new LabelStyle("\"Arial\"", 36, 255, 255, 255, 255, false, 0,
		0, 0, 178, 650, 80, "Bold", "Center", 13);
	h2 = new LabelStyle("\"Arial\"", 20, 244, 147, 31, 255, false, 0,
		0, 0, 229, 250, 35, "Bold", "Center", 2);
	bod = new LabelStyle("\"Arial\"", 12, 255, 255, 255, 255, false, 0,
		0, 0, 178, 250, 255, "Regular", "Left", 10);
	lab = new LabelStyle("\"Arial\"", 12, 0, 0, 0, 255, false, 255,
		255, 255, 255, 175, 25, "Bold", "Center", 4);
	img = new LabelStyle("\"Arial\"", 12, 0, 0, 0, 255, false, 255,
		255, 255, 255, 455, 325, "Italic", "Center", 10);
}

Narrative2::Narrative2(const Narrative2 & n, const osg::CopyOp & copyop)
	: Group(n, copyop),
	m_title(n.m_title),
	m_description(n.m_description),
	m_author(n.m_author),
	m_locked(n.m_locked),
	h1(n.h1), h2(n.h2), bod(n.bod), lab(n.lab), img(n.img)
{
	h1 = new LabelStyle("\"Arial\"", 36, 255, 255, 255, 255, false, 0,
		0, 0, 178, 650, 80, "Bold", "Center", 13);
	h2 = new LabelStyle("\"Arial\"", 20, 244, 147, 31, 255, false, 0,
		0, 0, 229, 250, 35, "Bold", "Center", 2);
	bod = new LabelStyle("\"Arial\"", 12, 255, 255, 255, 255, false, 0,
		0, 0, 178, 250, 255, "Regular", "Left", 10);
	lab = new LabelStyle("\"Arial\"", 12, 0, 0, 0, 255, false, 255,
		255, 255, 255, 175, 25, "Bold", "Center", 4);
	img = new LabelStyle("\"Arial\"", 12, 0, 0, 0, 255, false, 255,
		255, 255, 255, 455, 325, "Italic", "Center", 10);
}

Narrative2::Narrative2(const Narrative * old)
	: Group(),
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

	h1 = new LabelStyle("\"Arial\"", 36, 255, 255, 255, 255, false, 0,
		0, 0, 178, 650, 80, "Bold", "Center", 13);
	h2 = new LabelStyle("\"Arial\"", 20, 244, 147, 31, 255, false, 0,
		0, 0, 229, 250, 35, "Bold", "Center", 2);
	bod = new LabelStyle("\"Arial\"", 12, 255, 255, 255, 255, false, 0,
		0, 0, 178, 250, 255, "Regular", "Left", 10);
	lab = new LabelStyle("\"Arial\"", 12, 0, 0, 0, 255, false, 255,
		255, 255, 255, 175, 25, "Bold", "Center", 4);
	img = new LabelStyle("\"Arial\"", 12, 0, 0, 0, 255, false, 255,
		255, 255, 255, 455, 325, "Italic", "Center", 10);
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