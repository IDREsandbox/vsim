#include "VSimRoot.h"
#include <iostream>

VSimRoot::VSimRoot() {
	qDebug() << "root constructor, adding children";
	m_narratives = new NarrativeGroup;
	m_narratives->setName("Narratives");

	m_models = new ModelGroup;
	m_models->setName("Models");
}
VSimRoot::VSimRoot(const VSimRoot& n, const osg::CopyOp& copyop)
	: VSimRoot() {}

VSimRoot::VSimRoot(osg::Group * old_group)
	: VSimRoot()
{
	qDebug() << "converting to vsim" << old_group->getNumChildren();
	// find ModelInformation

	// NarrativeGroup
	// The conversion constructor removes Narratives from old_group so we don't worry about it
	m_narratives = new NarrativeGroup(old_group);
	m_narratives->setName("Narratives");

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
		}
	}

	qInfo() << "Models:" << m_models->getNumChildren();
	for (uint i = 0; i < m_models->getNumChildren(); i++) {
		qInfo() << "Model" << QString::fromStdString(m_models->getChild(i)->getName());
	}

	DebugVisitor v;
	m_models->accept(v);

	qDebug() << "Adding some data mappings" << m_models->dataTable();
	ModelData *data = new ModelData;
	data->setYearEnd(1000);
	data->setYearBegin(100);
	m_models->dataTable()->addMapping(this, data);
	ModelData *data2 = new ModelData;
	data->setYearEnd(4949);
	data->setYearBegin(111);
	m_models->dataTable()->addMapping(this, data2);
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
