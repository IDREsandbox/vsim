#include "VSimRoot.h"

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

	// find EResources
	// findOrCreateChildGroup(root, "EmbeddedResources");

	// Scan the root for different things, convert them, put them into the Narratives group
	uint numChildren = old_group->getNumChildren();
	for (uint i = 0; i < numChildren; i++) {
		osg::Node *node = old_group->getChild(i);

		if (!node) {
			qDebug() << "child" << i << "is null ptr?";
		}

		qDebug() << "found in root -" << QString::fromStdString(node->className());

		Narrative *old_narrative = dynamic_cast<Narrative*>(node);
		if (old_narrative) {
			qDebug() << "Found an old narrative" << QString::fromStdString(old_narrative->getName()) << "- converting";
			Narrative2 *new_narrative = new Narrative2(old_narrative);

			m_narratives->addChild(new_narrative);
		}
		// don't know the type? just copy to the model group
		else {
			m_models->addChild(node);
		}

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
			NarrativeSlide *slide = dynamic_cast<NarrativeSlide*>(nar->getChild(i));
			qInfo() << "\tSlide" << j << slide->getTransitionDuration();
		}
	}

	qInfo() << "Models:" << m_models->getNumChildren();
	for (uint i = 0; i < m_models->getNumChildren(); i++) {
		qInfo() << "Model" << QString::fromStdString(m_models->getChild(i)->getName());
	}
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

	osg::Group *other_models = other->models();
	for (uint i = 0; i < other_models->getNumChildren(); i++) {
		m_models->addChild(other_models->getChild(i));
	}

	// what are we supposed to do with all of the other children? copy them over?
	for (uint i = 0; i < other->getNumChildren(); i++) {
		osg::Node *node = other->getChild(i);
		addChild(node);
	}
}
