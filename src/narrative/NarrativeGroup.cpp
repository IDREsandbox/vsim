#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "deprecated/narrative/NarrativeOld.h"
#include <QDebug>

NarrativeGroup::NarrativeGroup() {
}

Narrative * NarrativeGroup::narrative(int index) const
{
	return child(index);
}
