#include "narrative/NarrativeSlideLabels.h"

NarrativeSlideLabels::NarrativeSlideLabels()
	: osg::Node(),
	m_rX(0.5),
	m_rY(0.5),
	m_rW(0.25),
	m_rH(0.2),
	m_text("New Label"),
	m_style("background: rgba(0, 0, 0, 70); color: rgb(255, 255, 255);")
{
}

NarrativeSlideLabels::NarrativeSlideLabels(const NarrativeSlideLabels & n, const osg::CopyOp & copyop)
	: osg::Node(n, copyop),
	m_rX(n.m_rX),
	m_rY(n.m_rY),
	m_rW(n.m_rW),
	m_rH(n.m_rH),
	m_text(n.m_text),
	m_style(n.m_style)
{
}

//TODO: add op to copy from legacy narr

NarrativeSlideLabels::~NarrativeSlideLabels()
{
}

float NarrativeSlideLabels::getrX() const
{
	return m_rX;
}

void NarrativeSlideLabels::setrX(float x)
{
	m_rX = x;
}

float NarrativeSlideLabels::getrY() const
{
	return m_rY;
}

void NarrativeSlideLabels::setrY(float y)
{
	m_rY = y;
}

float NarrativeSlideLabels::getrW() const
{
	return m_rW;
}

void NarrativeSlideLabels::setrW(float w)
{
	m_rW = w;
}

float NarrativeSlideLabels::getrH() const
{
	return m_rH;
}

void NarrativeSlideLabels::setrH(float h)
{
	m_rH = h;
}

const std::string& NarrativeSlideLabels::getText() const
{
	return m_text;
}

void NarrativeSlideLabels::setText(const std::string& text)
{
	m_text = text;
}

const std::string& NarrativeSlideLabels::getStyle() const
{
	return m_style;
}

void NarrativeSlideLabels::setStyle(const std::string& style)
{
	m_style = style;
}