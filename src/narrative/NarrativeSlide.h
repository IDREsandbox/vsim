#ifndef NARRATIVENODE_H_
#define NARRATIVENODE_H_

#include <osg/group>
#include <osg/image>
#include <QDebug>
#include <QImage>

class NarrativeSlide: public osg::Group
{
public:
    //enum NarrativeNodeFlags { NONE, WITH_OVERLAY_CANVAS };

    NarrativeSlide(int flags = 0);
	NarrativeSlide(const NarrativeSlide& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~NarrativeSlide();

	META_Node(, NarrativeSlide);

	// This is actually the camera matrix, gotten from manipulator->getMatrix()
	// the name is kept for compatibility
    const osg::Matrixd& getCameraMatrix() const;
	void setCameraMatrix(const osg::Matrixd& matrix);

	//this can own the widget data. 3 vecs: x, y, text
    //VSCanvas* getOverlayCanvas();
    //void setOverlayCanvas(VSCanvas* canvas);
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

#endif /* NARRATIVENODE_H_ */
