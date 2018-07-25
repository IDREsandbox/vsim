#include "SwitchManager.h"
#include "SwitchListModel.h"

#include <QDebug>

SwitchManager::SwitchManager(QObject * parent)
	: QObject(parent),
	m_observer(this)
{
	m_list_model = new SwitchListModel(this);
}

void SwitchManager::clear()
{
	m_list_model->clear();
}

void SwitchManager::addSwitch(osg::Switch * sw)
{
	m_list_model->addSwitch(sw);
	track(sw);
}

void SwitchManager::addMultiSwitch(osgSim::MultiSwitch * ms)
{
	m_list_model->addMultiSwitch(ms);
	track(ms);
}

void SwitchManager::removeNodeInternal(osg::Node * node, bool valid)
{
	m_list_model->removeNode(node);
	if (node && valid) {
		m_observer.remove(node);
	}
}


void SwitchManager::addNodeRecursive(osg::ref_ptr<osg::Node> node)
{
	SwitchInitVisitor visitor(this);
	node->accept(visitor);
}

void SwitchManager::removeNode(osg::Node * node)
{
	removeNodeInternal(node, false);
}

void SwitchManager::removeNodeRecursive(osg::ref_ptr<osg::Node> node)
{
	if (!node) return;
	// we have to find of the switches and things
	// remove observers
	// remove node from model
	auto visitor = SwitchRemoveVisitor(this);
	node->accept(visitor);
}

SwitchListModel *SwitchManager::listModel()
{
	return m_list_model;
}

void SwitchManager::track(osg::Node * node)
{
	m_observer.observe(node);
}

SwitchManager::SwitchInitVisitor::SwitchInitVisitor(SwitchManager *boss)
	: osg::NodeVisitor(osg::NodeVisitor::TraversalMode::TRAVERSE_ALL_CHILDREN),
	m_manager(boss)
{
}
void SwitchManager::SwitchInitVisitor::apply(osg::Switch & node)
{
	m_manager->addSwitch(&node);
	traverse(node);
}
void SwitchManager::SwitchInitVisitor::apply(osg::Group & group)
{
	auto *ms = dynamic_cast<osgSim::MultiSwitch*>(&group);
	if (ms) {
		m_manager->addMultiSwitch(ms);
	}
	traverse(group);
}

SwitchManager::SwitchRemoveVisitor::SwitchRemoveVisitor(SwitchManager *boss)
	: osg::NodeVisitor(osg::NodeVisitor::TraversalMode::TRAVERSE_ALL_CHILDREN),
	m_manager(boss)
{
}
void SwitchManager::SwitchRemoveVisitor::apply(osg::Switch &node)
{
	m_manager->removeNodeInternal(&node, true);
	traverse(node);
}
void SwitchManager::SwitchRemoveVisitor::apply(osg::Group &group)
{
	auto *ms = dynamic_cast<osgSim::MultiSwitch*>(&group);
	if (ms) {
		m_manager->removeNodeInternal(ms, true);
	}
	traverse(group);
}

SwitchManager::SwitchObserver::SwitchObserver(SwitchManager * boss)
	: m_manager(boss)
{
}

void SwitchManager::SwitchObserver::objectDeleted(void * obj)
{
	osg::Node *node = static_cast<osg::Node*>(obj);
	m_manager->removeNode(node);
}

