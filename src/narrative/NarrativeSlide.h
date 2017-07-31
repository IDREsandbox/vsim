#ifndef NARRATIVESLIDE_H
#define NARRATIVESLIDE_H

#include <osg/Group>
#include <osg/Image>
#include <QDebug>
#include <QImage>
#include "deprecated/narrative/NarrativeNode.h"

class NarrativeSlide: public osg::Group
{
public:	
	NarrativeSlide();
	NarrativeSlide(const NarrativeSlide& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	NarrativeSlide(const NarrativeNode *old, const NarrativeTransition *old_transition);
	virtual ~NarrativeSlide();
	
	META_Node(, NarrativeSlide);
	
	const osg::Matrixd& getCameraMatrix() const;
	void setCameraMatrix(const osg::Matrixd& matrix);
	
	float getDuration() const;
	void setDuration(float duration);
	bool getStayOnNode() const;
	void setStayOnNode(bool stay);
	const osg::Image *getThumbnail() const;
	void setThumbnail(osg::Image *thumbnail);

	float getTransitionDuration() const;
	void setTransitionDuration(float tduration);

protected:
	osg::Matrixd m_camera_matrix;
	float m_duration;
	bool m_stay_on_node;
	float m_transition_duration;
	osg::ref_ptr<osg::Image> m_thumbnail;
};

#endif /* NARRATIVESLIDE_H */
