#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"
#include <QDebug>

ECategoryGroup::ECategoryGroup()
{
}

ECategory * ECategoryGroup::category(int index) const
{
	return dynamic_cast<ECategory*>(child(index));
}


