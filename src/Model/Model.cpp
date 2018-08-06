#include "Model.h"
#include <osg/ValueObject>
#include <QDebug>

Model::Model(QObject *parent)
	: QObject(parent),
	m_node(nullptr),
	m_embedded(true),
	m_failed_to_load(true)
{
}

void Model::copyReference(const Model &other)
{
	m_node = other.m_node; // reference copy

	m_path = other.m_path;
	m_name = other.m_name;
	m_embedded = other.m_embedded;
	m_failed_to_load = other.m_failed_to_load;
}

//void Model::setNode(osg::Node * node)
//{
//	m_node = node;
//	emit sNodeChanged();
//}

osg::Node *Model::node() const
{
	return m_node.get();
}

osg::ref_ptr<osg::Node> Model::nodeRef() const
{
	return m_node;
}

bool Model::failedToLoad() const
{
	return m_failed_to_load;
}

void Model::setFailedToLoad(bool failed)
{
	m_failed_to_load = failed;
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

//void Model::setPath(const std::string &path)
//{
//	m_path = path;
//	emit sChanged();
//}

//bool Model::relativePath() const
//{
//	return m_relative;
//}
//
//void Model::setRelativePath(bool relative)
//{
//	m_relative;
//}

bool Model::embedded() const
{
	return m_embedded;
}

void Model::embedModel(osg::ref_ptr<osg::Node> node)
{
	m_embedded = true;
	m_path = "";

	if (m_node == node) {
		m_failed_to_load = (m_node == nullptr);
		return;
	}

	if (node == nullptr) {
		m_failed_to_load = true;
		m_node = new osg::Node;
	}
	else {
		m_failed_to_load = false;
		m_node = node;
	}
	emit sNodeChanged();
}

void Model::setFile(const std::string & path, osg::ref_ptr<osg::Node> node)
{
	m_embedded = false;
	m_path = path;

	if (m_node == node) {
		return;
	}

	if (node == nullptr) {
		m_failed_to_load = true;
		m_node = new osg::Node;
	}
	else {
		m_node = node;
		m_failed_to_load = false;
	}
	emit sNodeChanged();
}
