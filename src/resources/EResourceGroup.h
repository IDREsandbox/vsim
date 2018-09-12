#ifndef ERESOURCEGROUP_H
#define ERESOURCEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include "Core/GroupTemplate.h"

class ECategoryGroup;
class EResource;
class ECategory;
class EResourcesList;

class EResourceGroup : public TGroup<EResource> {
	Q_OBJECT

public:
	EResourceGroup(QObject *parent = nullptr);
	~EResourceGroup();

	void operator=(const EResourceGroup &other);

	void loadOld(const EResourcesList *old_ers);

	ECategoryGroup *categories() const;

	EResource *getResource(int) const;

	// assigns categories based on indices
	// stored by saveCategoryIndices()
	void restoreCategories() const;

	// stores category indices in resources
	// allows you to use EResource::getCategoryIndex()
	void saveCategoryIndices() const;

	void fixRelativePaths(const QString &old_base, const QString &new_base);

	void debug();

public: // commands

	// adds merge operations as children to cmd, for importing
	static void mergeCommand(EResourceGroup *group,
		const EResourceGroup *other, QUndoCommand *cmd);

private:
	ECategoryGroup *m_categories;
	bool m_restrict_to_current;
};

#endif // ERESOURCEGROUP_H