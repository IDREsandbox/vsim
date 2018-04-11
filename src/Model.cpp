#include "Model.h"
#include <osg/ValueObject>

Model::Model(QObject *parent)
	: QObject(parent),
	m_node(nullptr),
	m_external(false)
{
}

void Model::setNode(osg::Node * node)
{
	m_node = node;
}

osg::Node *Model::node() const
{
	return m_node.get();
}

void Model::setNodeYear(osg::Node *node, int year, bool begin)
{
	std::string prop;
	if (begin) prop = "yearBegin";
	else prop = "yearEnd";

	if (year == 0) {
		osg::UserDataContainer *cont = node->getUserDataContainer();
		cont->removeUserObject(cont->getUserObjectIndex(prop));
	}
	else {
		node->setUserValue(prop, year);
	}
	emit sNodeYearChanged(node, year, begin);
}

std::string Model::name() const
{
	return m_name;
}

void Model::setName(const std::string &path)
{
	m_name = path;
	emit sChanged();
}

std::string Model::path() const
{
	return m_path;
}

void Model::setPath(const std::string &path)
{
	m_path = path;
	emit sChanged();
}