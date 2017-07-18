/*
 * Narrative.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: eduardo
 */
#include <osg/node>
#include <assert.h>
#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"

Narrative2::Narrative2()
	: osg::Group(),
	m_title("Unknown"),
	m_description("Unknown"),
	m_author("Unknown"),
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