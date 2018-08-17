#include "narrative/NarrativeGroup.h"

#include <QDebug>

#include "narrative/Narrative.h"
#include "deprecated/narrative/NarrativeOld.h"
#include "Core/LockTable.h"

NarrativeGroup::NarrativeGroup(QObject *parent)
	: TGroup<Narrative>(parent)
{
}

Narrative * NarrativeGroup::narrative(int index) const
{
	return child(index);
}

//void NarrativeGroup::lockAll(QString password, int *out_success, int *out_fail)
//{
//	std::vector<LockTable*> lock_me;
//	for (auto nar_ptr : *this) {
//		if (!nar_ptr->lockTable()->isLocked()) {
//			lock_me.push_back(nar_ptr->lockTable());
//		}
//	}
//	LockTable::massLock(lock_me, password, out_success, out_fail);
//}
//
