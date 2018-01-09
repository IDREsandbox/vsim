/*
 * NarrativeNode.h
 *
 *  Created on: Feb 22, 2011
 *      Author: eduardo
 */
#ifndef NARRATIVENODE_H_
#define NARRATIVENODE_H_

#include <osg/Group>
#include <osg/Image>

class VSCanvas;
namespace osgNewWidget { class Canvas; }

class NarrativeNode: public osg::Group
{
public:
    enum NarrativeNodeFlags { NONE, WITH_OVERLAY_CANVAS };

    NarrativeNode(int flags = 0);
    NarrativeNode(const NarrativeNode& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
		: osg::Group(n, copyop),
		m_view_matrix(n.m_view_matrix),
		m_pauseAtNode(n.m_pauseAtNode),
		m_stayOnNode(n.m_stayOnNode) {}
    virtual ~NarrativeNode();

    META_Node(, NarrativeNode)

    osg::Matrixd& getViewMatrix() { return m_view_matrix; }
    const osg::Matrixd& getViewMatrix() const { return m_view_matrix; }
    void setViewMatrix(const osg::Matrixd& matrix) { m_view_matrix = matrix; }
    VSCanvas* getOverlayCanvas();
    void setOverlayCanvas(VSCanvas* canvas);
    float getPauseAtNode() const { return m_pauseAtNode; }
    void setPauseAtNode(float pause) { m_pauseAtNode = pause; }
	bool getStayOnNode() const { return m_stayOnNode; }
	void setStayOnNode(bool value) { m_stayOnNode = value; }
	// Both const and non-const versions are required for serializer.
	osg::Image* getImage() { return m_image; }
	const osg::Image* getImage() const { return m_image; }
	void setImage(osg::Image* image) { m_image = image; }

protected:
    osg::Matrixd m_view_matrix;
    float m_pauseAtNode;
	bool m_stayOnNode;
	osg::ref_ptr<osg::Image> m_image;
};

class NarrativeTransition: public osg::Node
{
public:
    NarrativeTransition();
    NarrativeTransition(const NarrativeTransition& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
    : osg::Node(n, copyop) {}
    virtual ~NarrativeTransition();

    META_Node(, NarrativeTransition)

    float getDuration() const { return m_duration; }
    void setDuration(float duration) { m_duration = duration; }

protected:

    float m_duration;
};

#endif /* NARRATIVENODE_H_ */
