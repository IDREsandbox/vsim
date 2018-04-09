#ifndef NARRATIVEGROUP_H
#define NARRATIVEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"
#include "GroupTemplate.h"

class Narrative;

class NarrativeGroup : public TGroup<Narrative> {
	Q_OBJECT
public:
	NarrativeGroup() {}
	NarrativeGroup(const osg::Group *old_group);

	Narrative *narrative(int index) const;
};

#endif