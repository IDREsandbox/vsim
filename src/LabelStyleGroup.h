#ifndef LABELSTYLEGROUP
#define LABELSTYLEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"
#include "LabelType.h"

class LabelStyle;

class LabelStyleGroup : public Group {
	Q_OBJECT;
public:
	LabelStyleGroup();
	LabelStyleGroup(const LabelStyleGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	META_Node(, LabelStyleGroup);

	void init();
	LabelStyle *getStyle(LabelType style) const;

private:
	LabelStyle *m_h1;
	LabelStyle *m_h2;
	LabelStyle *m_bod;
	LabelStyle *m_lab;
	//osg::ref_ptr<LabelStyle> m_img;
};

#endif // LABELSTYLEGROUP_H