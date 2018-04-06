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
	m_categories = new ECategoryGroup;
	m_categories->setName("Categories");
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
		ECategory *cat = new ECategory;
		cat->setCategoryName(old->getCategoryName());
		cat->setColor(QColor(old->getRed(), old->getGreen(), old->getBlue()));

		name_map.insert(std::make_pair(old->getCategoryName(), cat));
		m_categories->addChild(cat);
	}

	// copy the data
	for (EResourcesNode *node : old_ers->m_list) {
		EResource *er = new EResource(node, name_map);
		addChild(er);
	}
}

ECategoryGroup * EResourceGroup::categories() const
{
	return m_categories;
}
const ECategoryGroup * EResourceGroup::getCategories() const
{
	return m_categories;
}
void EResourceGroup::setCategories(ECategoryGroup * categories)
{
	m_categories = categories;
}

EResource * EResourceGroup::getResource(int i) const
{
	if (i >= (int)getNumChildren() || i < 0) return nullptr;
	return dynamic_cast<EResource*>(child(i));
}

void EResourceGroup::preSave()
{
	// index map
	std::unordered_map<ECategory*, size_t> cat_to_index;
	for (size_t i = 0; i < m_categories->getNumChildren(); i++) {
		ECategory *cat = m_categories->category(i);
		if (!cat) continue;
		cat_to_index[cat] = i;
	}
	// set the integer pointers
	for (size_t i = 0; i < getNumChildren(); i++) {
		EResource *res = getResource(i);
		if (!res) continue;
		ECategory *cat = res->category();
		int cat_index;
		if (cat) cat_index = (int)cat_to_index[cat];
		else cat_index = -1;
		res->setCategoryIndex(cat_index);
	}
}

void EResourceGroup::postLoad()
{
	// set categories
	for (size_t i = 0; i < getNumChildren(); i++) {
		EResource *res = getResource(i);
		if (!res) continue;

		// old resources already have these assigned
		ECategory *old_cat = res->category();
		if (old_cat) {
			continue;
		}

		int cat_index = res->getCategoryIndex();
		if (cat_index < 0 || cat_index >= (int)m_categories->getNumChildren()) {
			res->setCategory(nullptr);
			continue;
		}

		ECategory *cat = m_categories->category(cat_index);
		res->setCategory(cat);
	}
}

void EResourceGroup::debug() {
	qInfo() << "Embedded Resources:" << getNumChildren();
	for (uint i = 0; i < getNumChildren(); i++) {
		EResource *er = dynamic_cast<EResource*>(getChild(i));
		if (!er) {
			qInfo() << "not an EResource";
			continue;
		}
		ECategory *cat = er->category();
		QString cat_name;
		if (cat) cat_name = cat->getCategoryName().c_str();
		qInfo() << "Resource" << i
			<< QString::fromStdString(er->getResourceName())
			<< "global:" << er->getGlobal()
			<< "cat:" << cat_name << cat;

	}
	const ECategoryGroup *cats = getCategories();
	qInfo() << "ER Categories:" << cats->getNumChildren();
	for (uint i = 0; i < cats->getNumChildren(); i++) {
		const ECategory *cat = dynamic_cast<const ECategory*>(cats->getChild(i));
		if (!cat) continue;
		qInfo() << "Category" << i << QString::fromStdString(cat->getCategoryName()) << cat;
	}
}

void EResourceGroup::mergeCommand(EResourceGroup *group,
	const EResourceGroup *other, QUndoCommand *cmd)
{
	// build existing category map
	std::map<std::string, ECategory*> cat_map;
	for (size_t i = 0; i < group->categories()->getNumChildren(); i++) {
		ECategory *cat = group->categories()->category(i);
		if (!cat) continue;
		cat_map[cat->getCategoryName()] = cat;
	}

	// copy categories that don't exist, replace categories that do
	for (size_t i = 0; i < other->categories()->getNumChildren(); i++) {
		ECategory *new_cat = other->categories()->category(i);
		if (!new_cat) continue;

		auto it = cat_map.find(new_cat->getCategoryName());
		if (it == cat_map.end()) { // category doesn't already exist
			auto *add_cat = new Group::AddNodeCommand(group->categories(),
				new_cat, -1, cmd);

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

	std::vector<EResource*> added;
	for (size_t i = 0; i < other->getNumChildren(); i++) {
		EResource *res = other->getResource(i);
		if (!res) continue;

		auto *add_res = new Group::AddNodeCommand(group, res, -1, cmd);
		added.push_back(res);
	}

}
