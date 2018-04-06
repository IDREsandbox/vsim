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
class EResourcesList;

class EResourceGroup : public Group {
	Q_OBJECT

public:
	EResourceGroup();
	EResourceGroup(const osg::Group *old_root);
	~EResourceGroup();

	void loadOld(const EResourcesList *old_ers);

	ECategoryGroup *categories() const;
	const ECategoryGroup *getCategories() const;
	void setCategories(ECategoryGroup *categories);

	EResource *getResource(int) const;

	void preSave();
	void postLoad();

	void debug();

public: // commands

	// adds merge operations as children to cmd
	static void mergeCommand(EResourceGroup *group,
		const EResourceGroup *other, QUndoCommand *cmd);

private:
	osg::ref_ptr<ECategoryGroup> m_categories;
};

#endif // ERESOURCEGROUP_H