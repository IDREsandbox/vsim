#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "deprecated/narrative/NarrativeOld.h"
#include <QDebug>

NarrativeGroup::NarrativeGroup(QObject *parent)
	: TGroup<Narrative>(parent)
{
}

Narrative * NarrativeGroup::narrative(int index) const
{
	return child(index);
}

void NarrativeGroup::lockAll(QString password)
{
}

