#ifndef NARRATIVEGROUP_H
#define NARRATIVEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Core/Command.h"
#include "Core/GroupTemplate.h"

class Narrative;

class NarrativeGroup : public TGroup<Narrative> {
	Q_OBJECT
public:
	NarrativeGroup(QObject *parent = nullptr);
	// operator= // default

	Narrative *narrative(int index) const;

	//void lockAll(QString password,
	//	int *out_success = nullptr,
	//	int *out_fail = nullptr);
	//void unlockAll();
	//void lockAllPermanent();
	//void lockAllWithHash(HashLock lock);
};

#endif