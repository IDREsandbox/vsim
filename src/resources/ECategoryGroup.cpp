#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"
#include <QDebug>

ECategoryGroup::ECategoryGroup()
{
}

ECategoryGroup::ECategoryGroup(const ECategoryGroup & n, const osg::CopyOp & copyop)
{
}

ECategory * ECategoryGroup::category(int index) const
{
	return dynamic_cast<ECategory*>(child(index));
}


