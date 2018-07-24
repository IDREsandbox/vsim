#include "SwitchManager.h"
#include "SwitchModel.h"


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
}

void SwitchManager::addNode(osg::Node * node)
{
}

void SwitchManager::addNodeRecursive(osg::Node * node)
{
}

void SwitchManager::removeNode(osg::Node * node)
{
}

SwitchListModel * SwitchManager::listModel()
{
	return m_list_model;
}

SwitchModel * SwitchManager::switchModel()
{
	return m_switch_model;
}

SwitchManager::SwitchInitVisitor::SwitchInitVisitor(SwitchManager *boss)
	: m_manager(boss)
{
}

void SwitchManager::SwitchInitVisitor::apply(osg::Switch & node)
{
	m_manager->addSwitch(&node);
	traverse(node);
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
