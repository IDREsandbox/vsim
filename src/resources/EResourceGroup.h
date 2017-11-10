#ifndef ERESOURCEGROUP_H
#define ERESOURCEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"
#include "resources/EResource.h"

class EResourceGroup : public Group {
	Q_OBJECT

public:
	EResourceGroup() {}
	EResourceGroup(const EResourceGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	EResourceGroup(osg::Group *old_group);
	META_Node(, EResourceGroup)

};

#endif // ERESOURCEGROUP_H