#ifndef NARRATIVE2_H
#define NARRATIVE2_H

#include <string>
#include <osg/Node>
#include "narrative/NarrativeSlide.h"
#include "deprecated/narrative/Narrative.h"

class Narrative2: public osg::Group
{
public:
	Narrative2();
	Narrative2(const Narrative2& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	Narrative2(const Narrative *old); // converts old narrative to a new one
	virtual ~Narrative2();

	META_Node(, Narrative2)

	const std::string& getTitle() const;
	void setTitle(const std::string& title);
	const std::string& getAuthor() const;
	void setAuthor(const std::string& author);
	const std::string& getDescription() const;
	void setDescription(const std::string& description);

	bool getLock() const{return m_locked;}
	void setLock(bool lock){ m_locked = lock;}

private:
	std::string m_title;
	std::string m_description;
	std::string m_author;
	bool m_locked;
};

#endif /* NARRATIVE2_H */
