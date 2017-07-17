/*
 * NarrativeNode.h
 *
 *  Created on: Feb 22, 2011
 *      Author: eduardo
 */

#ifndef NARRATIVENODE_H_
#define NARRATIVENODE_H_

#include <osg/group>
#include <osg/image>
#include <QDebug>
#include <QImage>

//class VSCanvas;
//namespace osgNewWidget { class Canvas; }

class NarrativeNode: public osg::Group
{
public:
    //enum NarrativeNodeFlags { NONE, WITH_OVERLAY_CANVAS };

    NarrativeNode(int flags = 0);
	NarrativeNode(const NarrativeNode& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~NarrativeNode();

    META_Node(, NarrativeNode)

	// This is actually the camera matrix, gotten from manipulator->getMatrix()
	// the name is kept for compatibility
    osg::Matrixd& getViewMatrix() { return m_view_matrix; }
    const osg::Matrixd& getViewMatrix() const { return m_view_matrix; }
    void setViewMatrix(const osg::Matrixd& matrix) { m_view_matrix = matrix; }

	//this can own the widget data. 3 vecs: x, y, text
    //VSCanvas* getOverlayCanvas();
    //void setOverlayCanvas(VSCanvas* canvas);
    float getPauseAtNode() const { return m_pauseAtNode; }
    void setPauseAtNode(float pause) { m_pauseAtNode = pause; }
	bool getStayOnNode() const { return m_stayOnNode; }
	void setStayOnNode(bool value) { m_stayOnNode = value; }
	osg::Image *getImage() { return m_image; }
	const osg::Image *getImage() const { return m_image; }
	void setImage(osg::Image *image) { m_image = image; }

	float getTransitionDuration() const;
	void setTransitionDuration(float tduration);

protected:
    osg::Matrixd m_view_matrix;
    float m_pauseAtNode;
	bool m_stayOnNode;

	float m_transition_duration;
	osg::ref_ptr<osg::Image> m_image;
};


// DEPRECATED - This class is used for reading in old vsim formats.
class NarrativeTransition: public osg::Node
{
public:
    NarrativeTransition();
	NarrativeTransition(const NarrativeTransition& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~NarrativeTransition();

    META_Node(, NarrativeTransition)

    float getDuration() const { return m_duration; }
    void setDuration(float duration) { m_duration = duration; }

protected:

    float m_duration;
};

#endif /* NARRATIVENODE_H_ */
