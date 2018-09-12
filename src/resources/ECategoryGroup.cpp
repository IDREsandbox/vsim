#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"
#include <QDebug>
ECategoryGroup::ECategoryGroup(QObject *parent)
	: TGroup<ECategory>(parent)
{
}

ECategory * ECategoryGroup::category(int index) const
{
	return child(index);
}

bool ECategoryGroup::insert(size_t index, const std::vector<std::shared_ptr<ECategory>>& nodes)
{
	bool ok = TGroup<ECategory>::insert(index, nodes);

	if (ok) {
		for (auto &ptr : nodes) {
			connect(ptr.get(), &ECategory::sCNameChanged, this, &ECategoryGroup::sAnyChange);
			connect(ptr.get(), &ECategory::sColorChanged, this, &ECategoryGroup::sAnyChange);
		}
		emit sAnyChange();
	}
	return ok;
}

bool ECategoryGroup::remove(size_t index, size_t count)
{
	// disconnect
	for (size_t i = index; i < index + count; i++) {
		disconnect(child(i), 0, this, 0);
	}

	bool ok = TGroup<ECategory>::remove(index, count);
	emit sAnyChange();
	return ok;
}

void ECategoryGroup::clear()
{
	for (size_t i = 0; i < size(); i++) {
		disconnect(child(i), 0, this, 0);
	}
	TGroup<ECategory>::clear();
	emit sAnyChange();
}
