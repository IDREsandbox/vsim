#include "narrative/NarrativeSlide.h"

NarrativeSlide::NarrativeSlide(int flags)
	: osg::Group(),
	m_camera_matrix(),
	m_duration(15.0f),
	m_stay_on_node(false),
	m_transition_duration(4.0f),
	m_thumbnail(NULL)
{
}

NarrativeSlide::NarrativeSlide(const NarrativeSlide & n, const osg::CopyOp & copyop)
	: osg::Group(n, copyop),
	m_camera_matrix(n.m_camera_matrix),
	m_duration(n.m_duration),
	m_stay_on_node(n.m_stay_on_node),
	m_transition_duration(n.m_transition_duration),
	m_thumbnail(n.m_thumbnail)
{
}

NarrativeSlide::~NarrativeSlide()
{
}

const osg::Matrixd & NarrativeSlide::getCameraMatrix() const
{
	return m_camera_matrix;
}

void NarrativeSlide::setCameraMatrix(const osg::Matrixd & matrix)
{
	m_camera_matrix = matrix;
}

float NarrativeSlide::getDuration() const
{
	return m_duration;
}

void NarrativeSlide::setDuration(float duration)
{
	if (duration < 0) {
		qWarning() << "NarrativeSlide cannot have a negative duration";
		return;
	}
	m_duration = duration;
}

bool NarrativeSlide::getStayOnNode() const
{
	return m_stay_on_node;
}

void NarrativeSlide::setStayOnNode(bool stay)
{
	m_stay_on_node = stay;
}

const osg::Image * NarrativeSlide::getThumbnail() const
{
	return m_thumbnail;
}

void NarrativeSlide::setThumbnail(osg::Image * thumbnail)
{
	m_thumbnail = thumbnail;
}

float NarrativeSlide::getTransitionDuration() const
{
	return m_transition_duration;
}
void NarrativeSlide::setTransitionDuration(float tduration)
{
	if (tduration < 0) {
		qWarning() << "NarrativeSlide cannot have a negative transition duration";
		return;
	}
	m_transition_duration = tduration;
}