/*
 * Canvas.h
 *
 *  Created on: May 4, 2011
 *	Author: eduardo
 */

#ifndef OSGNW_CANVAS_H_
#define OSGNW_CANVAS_H_

#include "deprecated/osgNewWidget/Widget.h"

namespace osgNewWidget
{

class Canvas: public Widget
{
public:
	Canvas(int flags = osgNewWidget::WIDGET_NONE);
	Canvas(const Canvas&, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	virtual ~Canvas();

	META_Node(osgNewWidget, Canvas);

	osg::Group* getGroup() { return m_group.get(); }
	const osg::Group* getGroup() const { return m_group.get(); }
	void setGroup(osg::Group* group) { m_group = group; }

protected:
	osg::ref_ptr<osg::Group> m_group;
	osg::Vec2f m_contentOffset;
	osg::Vec2f m_parentScissorPos;
	osg::Vec2f m_parentScissorSize;
};

}

#endif /* OSGNW_CANVAS_H_ */
