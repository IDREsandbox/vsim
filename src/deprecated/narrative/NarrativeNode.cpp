/*
 * NarrativeNode.cpp
 *
 *  Created on: Feb 22, 2011
 *	  Author: eduardo
 */

#include "assert.h"
#include "deprecated/narrative/NarrativeNode.h"
#include "deprecated/osgNewWidget/VSWidget.h"

// ---------------------------------------------------
// NarrativeNode

NarrativeNode::NarrativeNode(int flags) : osg::Group(), m_view_matrix(), m_pauseAtNode(4.0f), m_stayOnNode(false), m_image(nullptr)
{
	//setNodeMask(0x00000000);
	if (flags == NarrativeNode::WITH_OVERLAY_CANVAS)
		setOverlayCanvas(new VSCanvas(osgNewWidget::WIDGET_NONE));
}

NarrativeNode::~NarrativeNode()
{
}

VSCanvas* NarrativeNode::getOverlayCanvas()
{
	int numChildren = getNumChildren();
	for (int i = 0; i < numChildren; i++)
	{
		osg::Node* n = getChild(i);
		if (n->getName() == "overlaycanvas")
			return dynamic_cast<VSCanvas*>(n);
	}
	assert(0);
	return NULL;
}

void NarrativeNode::setOverlayCanvas(VSCanvas* canvas)
{
	int numChildren = getNumChildren();
	for (int i = 0; i < numChildren; i++)
	{
		osg::Node* n = getChild(i);
		if (n->getName() == "overlaycanvas")
		{
			assert(0);
		}
	}
	canvas->setName("overlaycanvas");
	addChild(canvas);
}

// ---------------------------------------------------
// NarrativeTransition

NarrativeTransition::NarrativeTransition(): osg::Node(), m_duration(4.0f)
{
}

NarrativeTransition::~NarrativeTransition()
{
}
