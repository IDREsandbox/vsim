#include "resources/EResourceGroup.h"
#include "deprecated/resources/EResourcesList.h"
#include "deprecated/resources/EResourcesNode.h"

#include "resources/ECategoryGroup.h"
#include "resources/EResource.h"
#include "resources/ECategory.h"
#include "Core/GroupCommands.h"

#include <QDebug>
#include <unordered_map>

EResourceGroup::EResourceGroup(QObject *parent)
	: TGroup<EResource>(parent)
{
	m_categories = new ECategoryGroup(this);

	connect(this, &GroupSignals::sInsertedMulti, this, [this]() {
		for (size_t i = 0; i < size(); i++) {
			child(i)->setIndex(i);
		}
	});
	connect(this, &GroupSignals::sRemovedMulti, this, [this]() {
		for (size_t i = 0; i < size(); i++) {
			child(i)->setIndex(i);
		}
	});
}

EResourceGroup::~EResourceGroup()
{
}

void EResourceGroup::operator=(const EResourceGroup & other)
{
	other.saveCategoryIndices();
	TGroup<EResource>::operator=(other); // group assignment
	// deep copy, pointer copy would be bad
	*m_categories = *other.m_categories;

	// EResources copy category indices, so we can restore them
	restoreCategories();
}

void EResourceGroup::loadOld(const EResourcesList * old_ers)
{
	std::map<std::string, std::shared_ptr<ECategory>> name_map;
	for (EResourcesNode *old : old_ers->m_list) {
		if (name_map.find(old->getCategoryName()) != name_map.end()) {
			// already in the map so skip
			continue;
		}
		auto cat = std::make_shared<ECategory>();
		cat->setCategoryName(old->getCategoryName());
		cat->setColor(QColor(old->getRed(), old->getGreen(), old->getBlue()));

		name_map.insert(std::make_pair(old->getCategoryName(), cat));
		m_categories->append(cat);

	}

	// copy the data
	for (EResourcesNode *node : old_ers->m_list) {
		auto er = std::make_shared<EResource>();
		er->loadOld(node);
		// assign category

		ECategory *cat = nullptr;
		auto it = name_map.find(node->getCategoryName());
		if (it != name_map.end()) {
			er->setCategory(it->second);
		}

		append(er);
	}
}

ECategoryGroup * EResourceGroup::categories() const
{
	return m_categories;
}

EResource * EResourceGroup::getResource(int i) const
{
	return child(i);
}

void EResourceGroup::restoreCategories() const
{
	int i = 0;
	for (auto er : *this) {
		int cat_index = er->getCategoryIndex();
		auto c = m_categories->childShared(cat_index);
		QString cn = "";
		if (c) cn = c->getCategoryName().c_str();
		er->setCategory(c);
		i++;
	}
}

void EResourceGroup::saveCategoryIndices() const
{
	// this really shouldn't be const...
	// but we need it for serializer
	std::unordered_map<ECategory*, size_t> cat_to_index;
	for (size_t i = 0; i < m_categories->size(); i++) {
		ECategory *cat = m_categories->category(i);
		if (!cat) continue;
		cat_to_index[cat] = i;
	}

	for (auto er : *this) {
		ECategory *cat = er->category();
		auto it = cat_to_index.find(cat);
		if (it == cat_to_index.end()) {
			er->setCategoryIndex(-1);
		}
		else {
			er->setCategoryIndex(it->second);
		}
	}
}

void EResourceGroup::debug() {
	qInfo() << "Embedded Resources:" << size();
	for (uint i = 0; i < size(); i++) {
		EResource *er = child(i);
		ECategory *cat = er->category();
		QString cat_name;
		if (cat) cat_name = cat->getCategoryName().c_str();
		qInfo() << "Resource" << i
			<< QString::fromStdString(er->getResourceName())
			<< "global:" << er->getGlobal()
			<< "cat:" << cat_name << cat;
	}
	const ECategoryGroup *cats = categories();
	qInfo() << "ER Categories:" << cats->size();
	for (uint i = 0; i < cats->size(); i++) {
		ECategory *cat = cats->child(i);
		qInfo() << "Category" << i << QString::fromStdString(cat->getCategoryName()) << cat;
	}
}

void EResourceGroup::mergeCommand(EResourceGroup *group,
	const EResourceGroup *other, QUndoCommand *cmd)
{
	// build existing category map
	std::map<std::string, std::shared_ptr<ECategory>> cat_map;
	for (size_t i = 0; i < group->categories()->size(); i++) {
		auto cat = group->categories()->childShared(i);
		cat_map[cat->getCategoryName()] = cat;
	}

	// copy categories that don't exist, replace categories that do
	for (size_t i = 0; i < other->categories()->size(); i++) {
		auto new_cat = other->categories()->childShared(i);

		auto it = cat_map.find(new_cat->getCategoryName());
		if (it == cat_map.end()) { // category doesn't already exist
			auto *add_cat = new AddNodeCommand<ECategory>(group->categories(),
				new_cat, -1, cmd);

			cat_map[new_cat->getCategoryName()] = new_cat;
		}
		else { // category already exists
			auto old_cat = it->second;
			// replace all node categories
			std::set<EResource*> resources = new_cat->resources();
			for (auto *res : resources) {
				res->setCategory(old_cat);
			}
		}
	}

	// copy resources
	for (size_t i = 0; i < other->size(); i++) {
		auto res = other->childShared(i);
		if (!res) continue;
		auto *add_res = new AddNodeCommand<EResource>(group, res, -1, cmd);
	}
}
