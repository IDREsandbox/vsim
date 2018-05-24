#ifndef ECATEGORYGROUP_H
#define ECATEGORYGROUP_H

#include "Core/GroupTemplate.h"

class EResourcesList;
class ECategory;

class ECategoryGroup : public TGroup<ECategory> {
	Q_OBJECT
public:
	ECategoryGroup(QObject *parent = nullptr);
	// operator=() // default

	ECategory *category(int index) const;
};

#endif