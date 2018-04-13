#include "resources/EResourceGroup.h"
#include "deprecated/resources/EResourcesList.h"
#include "deprecated/resources/EResourcesNode.h"

#include "resources/ECategoryGroup.h"
#include "resources/EResource.h"
#include "resources/ECategory.h"

#include "GroupCommands.h"

#include <QDebug>
#include <unordered_map>

EResourceGroup::EResourceGroup()
{
	m_categories = std::unique_ptr<ECategoryGroup>(new ECategoryGroup);
}

EResourceGroup::~EResourceGroup()
{
}

void EResourceGroup::loadOld(const EResourcesList * old_ers)
{
	std::map<std::string, std::shared_ptr<ECategory>> name_map;
	for (EResourcesNode *old : old_ers->m_list) {
		if (name_map.find(old->getCategoryName()) != name_map.end()) {
			// already in the map so skip
			continue;
		}
		auto cat = std::shared_ptr<ECategory>(new ECategory);
		cat->setCategoryName(old->getCategoryName());
		cat->setColor(QColor(old->getRed(), old->getGreen(), old->getBlue()));

		name_map.insert(std::make_pair(old->getCategoryName(), cat));
		m_categories->append(cat);

	}

	// copy the data
	for (EResourcesNode *node : old_ers->m_list) {
		auto er = std::make_shared<EResource>(node);
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
	return m_categories.get();
}

EResource * EResourceGroup::getResource(int i) const
{
	return child(i);
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
