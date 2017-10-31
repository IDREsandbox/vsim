#include "VSimRoot.h"
#include <iostream>
#include "narrative/NarrativeSlideLabels.h"

VSimRoot::VSimRoot() {
	qDebug() << "root constructor, adding children";
	m_narratives = new NarrativeGroup;
	m_narratives->setName("Narratives");

	m_models = new ModelGroup;
	m_models->setName("Models");
}
VSimRoot::VSimRoot(const VSimRoot& n, const osg::CopyOp& copyop)
	: VSimRoot() 
{
	qDebug() << "vs root copy op";
}

VSimRoot::VSimRoot(osg::Group * old_group)
	: VSimRoot()
{
	qDebug() << "converting to vsim" << old_group->getNumChildren();
	// find ModelInformation

	// NarrativeGroup
	// The conversion constructor removes Narratives from old_group so we don't worry about it
	m_narratives = new NarrativeGroup(old_group);
	m_narratives->setName("Narratives");

	m_resources = new EResourceGroup(old_group);
	m_resources->setName("Resources");

	// The old categories worked by scanning all of the resources
	m_categories = new ECategoryGroup(m_resources.get());
	m_categories->setName("ERCategories");

	// Move everything unknown to the model group for displaying
	for (uint i = 0; i < old_group->getNumChildren(); i++) {
		osg::Node *node = old_group->getChild(i);
		m_models->addChild(old_group->getChild(i));
	}
}

NarrativeGroup * VSimRoot::narratives() const
{
	return m_narratives;
}
const NarrativeGroup *VSimRoot::getNarratives() const
{
	return m_narratives;
}
void VSimRoot::setNarratives(NarrativeGroup * narratives)
{
	m_narratives = narratives;
}
ModelGroup * VSimRoot::models() const
{
	return m_models;
}
const ModelGroup *VSimRoot::getModels() const
{
	return m_models;
}
void VSimRoot::setModels(ModelGroup * models)
{
	m_models = models;
}

EResourceGroup * VSimRoot::resources() const
{
	return m_resources;
}

const EResourceGroup * VSimRoot::getResources() const
{
	return m_resources;
}

void VSimRoot::setResources(EResourceGroup * resources)
{
	m_resources = resources;
}

ECategoryGroup * VSimRoot::categories() const
{
	return m_categories;
}

const ECategoryGroup * VSimRoot::getCategories() const
{
	return m_categories;
}

void VSimRoot::setCategories(ECategoryGroup * categories)
{
	m_categories = categories;
}

int VSimRoot::getFoo() const
{
	return 3;
}

void VSimRoot::setFoo(int x)
{
}
int VSimRoot::getFoob() const
{
	return 2;
}
void VSimRoot::setFoob(int x)
{
}
int VSimRoot::getFoox() const
{
	return foox;
}
void VSimRoot::setFoox(int x)
{
	foox = x;
}
void VSimRoot::debug()
{
	qInfo() << "root";
	for (uint i = 0; i < getNumChildren(); i++) {
		osg::Node *node = getChild(i);
		qInfo() << "-" << QString::fromStdString(node->getName()) << node->className();
	}

	qInfo() << "Narratives:" << m_narratives->getNumChildren();
	for (uint i = 0; i < m_narratives->getNumChildren(); i++) {
		Narrative2 *nar = dynamic_cast<Narrative2*>(m_narratives->getChild(i));
		if (!nar) continue;
		qInfo() << "Narrative" << i << QString::fromStdString(nar->getTitle());
		for (uint j = 0; j < nar->getNumChildren(); j++) {
			NarrativeSlide *slide = dynamic_cast<NarrativeSlide*>(nar->getChild(j));
			qInfo() << "\tSlide" << j << slide->getTransitionDuration();

			for (uint k = 0; k < slide->getNumChildren(); k++) {
				NarrativeSlideLabels *label = dynamic_cast<NarrativeSlideLabels*>(slide->getChild(k));
				qInfo() << "\t\tLabel" << k << QString::fromStdString(label->getText());
			}
		}
	}
	qInfo() << "Embedded Resources:" << m_resources->getNumChildren();
	for (uint i = 0; i < m_resources->getNumChildren(); i++) {
		EResource *er = dynamic_cast<EResource*>(m_resources->getChild(i));
		if (!er) continue;
		qInfo() << "Resource" << i << QString::fromStdString(er->getResourceName());
	}
	qInfo() << "Categories:" << m_categories->getNumChildren();
	for (uint i = 0; i < m_categories->getNumChildren(); i++) {
		ECategory *cat = dynamic_cast<ECategory*>(m_categories->getChild(i));
		if (!cat) continue;
		qInfo() << "Category" << i << QString::fromStdString(cat->getCategoryName());
	}
	qInfo() << "Models:" << m_models->getNumChildren();
	for (uint i = 0; i < m_models->getNumChildren(); i++) {
		qInfo() << "Model" << QString::fromStdString(m_models->getChild(i)->getName());
	}

	DebugVisitor v;
	m_models->accept(v);
}

void VSimRoot::merge(VSimRoot *other)
{
	// basically just copy over the Narratives and Models contents
	qDebug() << "merging";

	NarrativeGroup *other_narratives = other->narratives();
	qDebug() << "current narratives:" << m_narratives->getNumChildren();
	qDebug() << "other narratives:" << other_narratives->getNumChildren();
	for (uint i = 0; i < other_narratives->getNumChildren(); i++) {
		m_narratives->addChild(other_narratives->getChild(i));
	}

	m_models->merge(other->models());

	// what are we supposed to do with all of the other children? copy them over?
	for (uint i = 0; i < other->getNumChildren(); i++) {
		osg::Node *node = other->getChild(i);
		addChild(node);
	}
}

void DebugVisitor::apply(osg::Group &group)
{
	std::cout << std::string(m_tabs, '\t');
	std::cout << group.className() << " " << group.getName() << " " << group.getNumChildren() << '\n';

	m_tabs++;
	traverse(group);
	m_tabs--;
}

void DebugVisitor::apply(osg::Node & node)
{
	std::cout << std::string(m_tabs, '\t') << node.className() << " " << node.getName() << "\n";
}
