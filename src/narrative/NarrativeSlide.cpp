#include "narrative/NarrativeSlide.h"

NarrativeSlide::NarrativeSlide()
	: osg::Group(),
	m_camera_matrix(),
	m_duration(15.0f),
	m_stay_on_node(false),
	m_transition_duration(4.0f)
{
}

NarrativeSlide::NarrativeSlide(const NarrativeSlide & n, const osg::CopyOp & copyop)
	: osg::Group(n, copyop),
	m_camera_matrix(n.m_camera_matrix),
	m_duration(n.m_duration),
	m_stay_on_node(n.m_stay_on_node),
	m_transition_duration(n.m_transition_duration)
{
}

NarrativeSlide::NarrativeSlide(const NarrativeNode * old, const NarrativeTransition * old_transition)
	: osg::Group(),
	m_camera_matrix(old->getViewMatrix()),
	m_duration(old->getPauseAtNode()),
	m_stay_on_node(old->getStayOnNode()),
	m_transition_duration(old_transition->getDuration())
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
	emit sCameraMatrixChanged(matrix);
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
	emit sDurationChanged(duration);
}

bool NarrativeSlide::getStayOnNode() const
{
	return m_stay_on_node;
}

void NarrativeSlide::setStayOnNode(bool stay)
{
	m_stay_on_node = stay;
	emit sStayOnNodeChanged(stay);
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
	emit sTransitionDurationChanged(tduration);
	qDebug() << "emit duration change";
}

NarrativeSlide::NewLabelCommand::NewLabelCommand(NarrativeSlide * slide, QUndoCommand * parent)
	: QUndoCommand(parent),
	m_slide(slide)
{
	m_label = new NarrativeSlideLabels;
}
void NarrativeSlide::NewLabelCommand::undo()
{
	m_slide->removeChild(m_label);
	m_slide->sDeleteLabel(m_label);
}
void NarrativeSlide::NewLabelCommand::redo()
{
	m_slide->addChild(m_label);
	m_slide->sNewLabel(m_label);
}

NarrativeSlide::DeleteLabelCommand::DeleteLabelCommand(NarrativeSlide * slide, NarrativeSlideLabels *label, QUndoCommand * parent)
	: QUndoCommand(parent),
	m_slide(slide),
	m_label(label)
{
}
void NarrativeSlide::DeleteLabelCommand::undo()
{
	m_slide->addChild(m_label);
	m_slide->sNewLabel(m_label);
}
void NarrativeSlide::DeleteLabelCommand::redo()
{
	m_slide->removeChild(m_label);
	m_slide->sDeleteLabel(m_label);
}
