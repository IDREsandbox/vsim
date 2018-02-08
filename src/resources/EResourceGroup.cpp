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
	qDebug() << "Creating EResourceGroup";
	m_categories = new ECategoryGroup;
	m_categories->setName("Categories");
}

EResourceGroup::EResourceGroup(const EResourceGroup & n, const osg::CopyOp & copyop)
{
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
		std::map<std::string, ECategory*> name_map;
		if (old_ers) {
			qInfo() << "Found an old ER List";

			for (EResourcesNode *old : old_ers->m_list) {
				if (name_map.find(old->getCategoryName()) != name_map.end()) {
					// already in the map so skip
					continue;
				}
				ECategory *cat = new ECategory;
				cat->setCategoryName(old->getCategoryName());
				cat->setRed(old->getRed());
				cat->setGreen(old->getGreen());
				cat->setBlue(old->getBlue());

				name_map.insert(std::make_pair(old->getCategoryName(), cat));
				m_categories->addChild(cat);
			}

			// copy the data
			for (EResourcesNode *node : old_ers->m_list) {
				EResource *er = new EResource(node, name_map);
				addChild(er);
			}

		}
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

EResource * EResourceGroup::getResource(int i)
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

		int cat_index = res->getCategoryIndex();
		if (cat_index < 0 || cat_index >= m_categories->getNumChildren()) {
			res->setCategory(nullptr);
			continue;
		}

		ECategory *cat = m_categories->category(cat_index);
		res->setCategory(cat);
		cat->addResource(res);
	}
}