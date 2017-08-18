#include "ModelData.h"

int ModelData::getYearBegin() const
{
	return m_year_begin;
}

void ModelData::setYearBegin(int year)
{
	if (year == 0) {
		return;
	}
	m_year_begin = year;
	emit sYearBeginChange(year);
}

int ModelData::getYearEnd() const
{
	return m_year_end;
}

void ModelData::setYearEnd(int year)
{
	if (year == 0) {
		return;
	}
	m_year_end = year;
	emit sYearEndChange(year);
}

ModelData * ModelData::addModelNode(osg::Group * g)
{
	if (!g) return nullptr;
	// look for existing model node
	for (uint i = 0; i < g->getNumChildren(); i++) {
		ModelData *node = dynamic_cast<ModelData*>(g->getChild(i));
		if (node) return node;
	}
	ModelData *new_node = new ModelData;
	g->addChild(new_node);
	return new_node;
}

void ModelData::removeModelNode(osg::Group * g)
{
	for (int i = g->getNumChildren() - 1; i > 0; i--) {
		ModelData *node = dynamic_cast<ModelData*>(g->getChild(i));
		if (node) g->removeChild(i);
	}
}

ModelData * ModelData::getModelNode(osg::Node * g)
{
	osg::Group *group = g->asGroup();
	if (!group) return nullptr;
	return getModelNode(group);
}

ModelData * ModelData::getModelNode(osg::Group *g)
{
	if (!g) return nullptr;
	// look for existing model node
	for (uint i = 0; i < g->getNumChildren(); i++) {
		ModelData *node = dynamic_cast<ModelData*>(g->getChild(i));
		if (node) return node;
	}
	return nullptr;
}
