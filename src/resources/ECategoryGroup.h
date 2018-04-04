#ifndef ECATEGORYGROUP_H
#define ECATEGORYGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Group.h"
#include <map>

class EResourcesList;
class ECategory;
class ECategoryGroup : public Group {
	Q_OBJECT
public:
	ECategoryGroup();

	ECategory *category(int index) const;
};

#endif