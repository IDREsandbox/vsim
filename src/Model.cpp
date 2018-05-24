#include "Model.h"
#include <osg/ValueObject>

Model::Model(QObject *parent)
	: QObject(parent),
	m_node(nullptr),
	m_external(false)
{
}

void Model::copyReference(const Model &other)
{
	m_node = other.m_node; // reference copy

	m_external = other.m_external;
	m_path = other.m_path;
	m_name = other.m_name;
}

void Model::setNode(osg::Node * node)
{
	m_node = node;
}

osg::Node *Model::node() const
{
	return m_node.get();
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