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
	EResourceGroup(const EResourceGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	EResourceGroup(const osg::Group *old_root);
	META_Node(, EResourceGroup);

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