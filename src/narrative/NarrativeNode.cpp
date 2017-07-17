/*
 * NarrativeNode.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: eduardo
 */

#include "narrative/NarrativeNode.h"

// ---------------------------------------------------
// NarrativeNode

NarrativeNode::NarrativeNode(int flags)
	: osg::Group(),
	m_view_matrix(),
	m_pauseAtNode(15.0f),
	m_transition_duration(4.0f),
	m_stayOnNode(false),
	m_image(NULL)
{
    //setNodeMask(0x00000000);
//    if (flags == NarrativeNode::WITH_OVERLAY_CANVAS)
//        setOverlayCanvas(new VSCanvas(osgNewWidget::WIDGET_NONE));
}

NarrativeNode::NarrativeNode(const NarrativeNode & n, const osg::CopyOp & copyop)
	: osg::Group(n, copyop),
	m_view_matrix(n.m_view_matrix),
	m_pauseAtNode(n.m_pauseAtNode),
	m_stayOnNode(n.m_stayOnNode),
	m_transition_duration(n.m_transition_duration),
	m_image(n.m_image)
{
}

NarrativeNode::~NarrativeNode()
{
}

float NarrativeNode::getTransitionDuration() const
{
	return m_transition_duration;
}
void NarrativeNode::setTransitionDuration(float tduration)
{
	if (tduration < 0) {
		qWarning() << "NarrativeNode cannot have a negative transition duration";
		return;
	}
	m_transition_duration = tduration;
}

//VSCanvas* NarrativeNode::getOverlayCanvas()
//{
//    int numChildren = getNumChildren();
//    for (int i = 0; i < numChildren; i++)
//    {
//        osg::Node* n = getChild(i);
//        if (n->getName() == "overlaycanvas")
//            return dynamic_cast<VSCanvas*>(n);
//    }
//    assert(0);
//    return NULL;
//}

//void NarrativeNode::setOverlayCanvas(VSCanvas* canvas)
//{
//    int numChildren = getNumChildren();
//    for (int i = 0; i < numChildren; i++)
//    {
//        osg::Node* n = getChild(i);
//        if (n->getName() == "overlaycanvas")
//        {
//            assert(0);
//        }
//    }
//    canvas->setName("overlaycanvas");
//    addChild(canvas);
//}

// ---------------------------------------------------
// NarrativeTransition

NarrativeTransition::NarrativeTransition()
	: osg::Node(), 
	m_duration(4.0f)
{
}

NarrativeTransition::NarrativeTransition(const NarrativeTransition & n, const osg::CopyOp & copyop)
	: osg::Node(n, copyop),
	m_duration(n.m_duration)
{  
}

NarrativeTransition::~NarrativeTransition()
{
}
