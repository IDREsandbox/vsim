#ifndef ECATEGORYGROUP_H
#define ECATEGORYGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Group.h"
#include "GroupTemplate.h"
#include <map>

class EResourcesList;
class ECategory;
class ECategoryGroup : public TGroup<ECategory> {
	Q_OBJECT
public:
	ECategoryGroup();

	ECategory *category(int index) const;
};

#endif