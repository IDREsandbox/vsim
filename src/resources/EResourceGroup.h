#ifndef ERESOURCEGROUP_H
#define ERESOURCEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Command.h"
#include "GroupTemplate.h"

class ECategoryGroup;
class EResource;
class ECategory;
class EResourcesList;

class EResourceGroup : public TGroup<EResource> {
	Q_OBJECT

public:
	EResourceGroup();
	EResourceGroup(const osg::Group *old_root);
	~EResourceGroup();

	void loadOld(const EResourcesList *old_ers);

	ECategoryGroup *categories() const;

	EResource *getResource(int) const;

	void debug();

public: // commands

	// adds merge operations as children to cmd
	static void mergeCommand(EResourceGroup *group,
		const EResourceGroup *other, QUndoCommand *cmd);

private:
	std::unique_ptr<ECategoryGroup> m_categories;
};

#endif // ERESOURCEGROUP_H