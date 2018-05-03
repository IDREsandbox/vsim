#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"
#include <QDebug>
#include "WeakObject.h"
ECategoryGroup::ECategoryGroup(QObject *parent)
	: TGroup<ECategory>(parent)
{
}

ECategory * ECategoryGroup::category(int index) const
{
	return child(index);
}


