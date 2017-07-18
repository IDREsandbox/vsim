/*
 * Canvas.cpp
 *
 *  Created on: May 4, 2011
 *	Author: eduardo
 */

#include "deprecated/osgNewWidget/Canvas.h"
#include <osg/Scissor>

namespace osgNewWidget
{

#define FOR_EACH_CHILD(x) \
	{ \
		int numChildren = m_group->getNumChildren(); \
		for (int i = 0; i < numChildren; i++) \
		{ \
			osg::Node* n = m_group->getChild(i); \
			osgNewWidget::Widget* (x) = dynamic_cast<osgNewWidget::Widget*>(n); \
			if (x) \
			{
#define END_FOR_EACH \
			} \
		} \
	}

Canvas::Canvas(int flags)
	: Widget(flags), 
	m_contentOffset(0.0f, 0.0f),
	m_parentScissorPos(0.0f, 0.0f), 
	m_parentScissorSize(FLT_MAX, FLT_MAX)
{
	m_group = new osg::Group();
	//m_group->setNodeMask(osgNewWidget::NODEMASK_2D);
	m_group->getOrCreateStateSet()->setAttributeAndModes(new osg::Scissor(0, 0, 0, 0),
		osg::StateAttribute::ON);
}

Canvas::Canvas(const Canvas& canvas, const osg::CopyOp& copyop)
	: Widget(canvas, copyop)
{
	m_group = new osg::Group(*canvas.m_group.get(), copyop);
}

Canvas::~Canvas()
{
}
}
