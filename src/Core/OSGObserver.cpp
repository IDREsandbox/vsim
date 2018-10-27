#include "OSGObserver.h"
#include <QDebug>

OSGObserver::OSGObserver()
	: m_observer(this)
{
}

OSGObserver::~OSGObserver()
{
	// make a copy for safe iteration
	std::set<osg::Referenced*> observed = m_observed;

	for (osg::Referenced *ref : observed) {
		remove(ref);
	}
}

void OSGObserver::observe(osg::Referenced *node)
{
	auto result = m_observed.insert(node);
	// duplicate
	if (!result.second) {
		return;
	}
	node->addObserver(&m_observer);
}

void OSGObserver::remove(osg::Referenced *node)
{
	node->removeObserver(&m_observer);
	m_observed.erase(node);
}

const std::set<osg::Referenced*> &OSGObserver::observed()
{
	return m_observed;
}

void OSGObserver::objectDeleted(void * obj)
{
}

OSGObserver::InternalObserver::InternalObserver(OSGObserver * boss)
	: m_boss(boss)
{
}

void OSGObserver::InternalObserver::objectDeleted(void * obj)
{
	m_boss->m_observed.erase((osg::Referenced*)obj);
	m_boss->objectDeleted(obj);
}
