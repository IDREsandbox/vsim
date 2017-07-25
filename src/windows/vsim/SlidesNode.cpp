#include "SlidesNode.h"

SlidesNode::SlidesNode(int flags)
	: osg::Group()
{
	//setNodeMask(0x00000000);
	//    if (flags == SlidesNode::WITH_OVERLAY_CANVAS)
	//        setOverlayCanvas(new VSCanvas(osgNewWidget::WIDGET_NONE));
}

SlidesNode::SlidesNode(const SlidesNode & n, const osg::CopyOp & copyop)
	: osg::Group(n, copyop),
	m_text(n.m_text),
	m_style(n.m_style),
	m_w(n.m_w), m_h(n.m_h), m_x(n.m_x), m_y(n.m_y)
{
}

SlidesNode::~SlidesNode()
{
}

float SlidesNode::getTransitionDuration() const
{
	return m_transition_duration;
}
void SlidesNode::setTransitionDuration(float tduration)
{
	if (tduration < 0) {
		qWarning() << "SlidesNode cannot have a negative transition duration";
		return;
	}
	m_transition_duration = tduration;
}

//VSCanvas* SlidesNode::getOverlayCanvas()
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

//void SlidesNode::setOverlayCanvas(VSCanvas* canvas)
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

NarrativeTransition::NarrativeTransition() : osg::Node(), m_duration(4.0f)
{
}

NarrativeTransition::~NarrativeTransition()
{
}
