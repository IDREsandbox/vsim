#include "resources/EResourceGroup.h"
#include "deprecated/resources/EResourcesList.h"
#include "deprecated/resources/EResourcesNode.h"

#include "resources/ECategoryGroup.h"
#include "resources/EResource.h"
#include "resources/ECategory.h"

#include <QDebug>
#include <unordered_map>

EResourceGroup::EResourceGroup()
{
	m_categories = std::unique_ptr<ECategoryGroup>(new ECategoryGroup);
}

EResourceGroup::EResourceGroup(const osg::Group *old_root)
	: EResourceGroup()
{
	for (int i = old_root->getNumChildren() - 1; i >= 0; i--) {
		const osg::Node *node = old_root->getChild(i);
		const EResourcesNode *old_EResource = dynamic_cast<const EResourcesNode*>(node);
		if (old_EResource) {
			qInfo() << "Found an old EResource" << QString::fromStdString(old_EResource->getName()) << "- adding to group";
		}

		const EResourcesList *old_ers = dynamic_cast<const EResourcesList*>(node);
		if (old_ers) loadOld(old_ers);
	}
}

EResourceGroup::~EResourceGroup()
{
}

void EResourceGroup::loadOld(const EResourcesList * old_ers)
{
	std::map<std::string, ECategory*> name_map;
	for (EResourcesNode *old : old_ers->m_list) {
		if (name_map.find(old->getCategoryName()) != name_map.end()) {
			// already in the map so skip
			continue;
		}
		auto cat = std::shared_ptr<ECategory>(new ECategory);
		cat->setCategoryName(old->getCategoryName());
		cat->setColor(QColor(old->getRed(), old->getGreen(), old->getBlue()));

		name_map.insert(std::make_pair(old->getCategoryName(), cat.get()));
		m_categories->append(cat);
	}

	// copy the data
	for (EResourcesNode *node : old_ers->m_list) {
		auto er = std::shared_ptr<EResource>(new EResource(node, name_map));
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
	std::map<std::string, ECategory*> cat_map;
	for (size_t i = 0; i < group->categories()->size(); i++) {
		ECategory *cat = group->categories()->category(i);
		if (!cat) continue;
		cat_map[cat->getCategoryName()] = cat;
	}

	// copy categories that don't exist, replace categories that do
	for (size_t i = 0; i < other->categories()->size(); i++) {
		ECategory *new_cat = other->categories()->category(i);
		if (!new_cat) continue;

		auto it = cat_map.find(new_cat->getCategoryName());
		if (it == cat_map.end()) { // category doesn't already exist
			auto *add_cat = new AddNodeCommand<ECategory>(group->categories(),
				other->categories()->childShared(i), -1, cmd);

			cat_map[new_cat->getCategoryName()] = new_cat;
		}
		else { // category already exists
			ECategory *old_cat = it->second;
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
