#include "NarrativeSlideLabels.h"

NarrativeSlideLabels::NarrativeSlideLabels()
	: osg::Node(),
	m_x(250),
	m_y(250),
	m_w(250),
	m_h(150),
	m_parH(600),
	m_parW(800),
	//m_ratH(.5),
	//m_ratW(.5),
	m_text("New Label"),
	m_style("background: rgba(0, 0, 0, 70); color: rgb(255, 255, 255);")
{
}

NarrativeSlideLabels::NarrativeSlideLabels(const NarrativeSlideLabels & n, const osg::CopyOp & copyop)
	: osg::Node(n, copyop),
	m_x(250),
	m_y(250),
	m_w(n.m_w),
	m_h(n.m_h),
	m_parH(n.m_parH),
	m_parW(n.m_parW),
	m_ratH(n.m_ratH),
	m_ratW(n.m_ratW),
	m_text(n.m_text),
	m_style(n.m_style)
{
}

//TODO: add op to copy from legacy narr

NarrativeSlideLabels::~NarrativeSlideLabels()
{
}

int NarrativeSlideLabels::getX() const
{
	return m_x;
}

void NarrativeSlideLabels::setX(int x)
{
	m_x = x;
}

int NarrativeSlideLabels::getY() const
{
	return m_y;
}

void NarrativeSlideLabels::setY(int y)
{
	m_y = y;
}

int NarrativeSlideLabels::getW() const
{
	return m_w;
}

void NarrativeSlideLabels::setW(int w)
{
	m_w = w;
}

int NarrativeSlideLabels::getH() const
{
	return m_h;
}

void NarrativeSlideLabels::setH(int h)
{
	m_h = h;
}

int NarrativeSlideLabels::getParW() const
{
	return m_parW;
}

void NarrativeSlideLabels::setParW(int pw)
{
	m_parW = pw;
}

int NarrativeSlideLabels::getParH() const
{
	return m_parH;
}

void NarrativeSlideLabels::setParH(int ph)
{
	m_parH = ph;
}

float NarrativeSlideLabels::getRatW() const
{
	return m_ratW;
}

void NarrativeSlideLabels::setRatW(float rw)
{
	m_ratW = rw;
}

float NarrativeSlideLabels::getRatH() const
{
	return m_ratH;
}

void NarrativeSlideLabels::setRatH(float rh)
{
	m_ratH = rh;
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