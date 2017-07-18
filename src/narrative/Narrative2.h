/*
 * Narrative.h
 *
 *  Created on: Feb 4, 2011
 *      Author: eduardo
 */

#ifndef NARRATIVE_H_
#define NARRATIVE_H_

#include <string>
#include <osg/node>
#include "narrative/NarrativeSlide.h"

class Narrative2: public osg::Group
{
public:
    Narrative2();
	Narrative2(const Narrative2& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
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

#endif /* NARRATIVE_H_ */
