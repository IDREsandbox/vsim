#ifndef NARRATIVEGROUP_H
#define NARRATIVEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"

class NarrativeGroup : public Group {
	Q_OBJECT
public:
	NarrativeGroup() {}
	NarrativeGroup(const osg::Group *old_group);
};

#endif