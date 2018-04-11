#ifndef ECATEGORYGROUP_H
#define ECATEGORYGROUP_H

#include "GroupTemplate.h"

class EResourcesList;
class ECategory;
class ECategoryGroup : public TGroup<ECategory> {
	Q_OBJECT
public:
	ECategoryGroup();

	ECategory *category(int index) const;
};

#endif