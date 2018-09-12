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

	bool insert(size_t index, const std::vector<std::shared_ptr<ECategory>> &nodes) override;
	bool remove(size_t index, size_t count) override;
	void clear() override;

signals:
	void sAnyChange();
};

#endif