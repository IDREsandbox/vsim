#ifndef NARRATIVEGROUP_H
#define NARRATIVEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "GroupTemplate.h"

class Narrative;

class NarrativeGroup : public TGroup<Narrative> {
	Q_OBJECT
public:
	NarrativeGroup();

	Narrative *narrative(int index) const;
};

#endif