#include "ModelNode.h"

int ModelNode::getYearBegin() const
{
	return m_year_begin;
}

void ModelNode::setYearBegin(int year)
{
	if (year == 0) {
		return;
	}
	m_year_begin = year;
	emit sYearBeginChange(year);
}

int ModelNode::getYearEnd() const
{
	return m_year_end;
}

void ModelNode::setYearEnd(int year)
{
	if (year == 0) {
		return;
	}
	m_year_end = year;
	emit sYearEndChange(year);
}

ModelNode * ModelNode::addModelNode(osg::Group * g)
{
	if (!g) return nullptr;
	// look for existing model node
	for (uint i = 0; i < g->getNumChildren(); i++) {
		ModelNode *node = dynamic_cast<ModelNode*>(g->getChild(i));
		if (node) return node;
	}
	ModelNode *new_node = new ModelNode;
	g->addChild(new_node);
	return new_node;
}

void ModelNode::removeModelNode(osg::Group * g)
{
	for (int i = g->getNumChildren() - 1; i > 0; i--) {
		ModelNode *node = dynamic_cast<ModelNode*>(g->getChild(i));
		if (node) g->removeChild(i);
	}
}
