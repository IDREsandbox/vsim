#ifndef LABELSTYLEGROUP
#define LABELSTYLEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"
#include "LabelStyle.h"

class LabelStyleGroup : public Group {
	Q_OBJECT;
public:
	LabelStyleGroup();
	LabelStyleGroup(const LabelStyleGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	META_Node(, LabelStyleGroup);

	void init();
	LabelStyle *getStyle(LabelStyle::Style style) const;

private:
	osg::ref_ptr<LabelStyle> m_h1;
	osg::ref_ptr<LabelStyle> m_h2;
	osg::ref_ptr<LabelStyle> m_bod;
	osg::ref_ptr<LabelStyle> m_lab;
	//osg::ref_ptr<LabelStyle> m_img;
};

#endif // LABELSTYLEGROUP_H