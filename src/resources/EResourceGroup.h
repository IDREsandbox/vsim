#ifndef ERESOURCEGROUP_H
#define ERESOURCEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "Group.h"

class ECategoryGroup;
class EResource;
class ECategory;

class EResourceGroup : public Group {
	Q_OBJECT

public:
	EResourceGroup();
	EResourceGroup(const osg::Group *old_root);

	ECategoryGroup *categories() const;
	const ECategoryGroup *getCategories() const;
	void setCategories(ECategoryGroup *categories);

	EResource *getResource(int);

	void preSave();
	void postLoad();

private:
	osg::ref_ptr<ECategoryGroup> m_categories;
};

#endif // ERESOURCEGROUP_H