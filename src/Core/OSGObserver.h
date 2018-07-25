#ifndef OSGOBSERVER_H
#define OSGOBSERVER_H

#include <osg/Observer>

// osg observers don't clean themselves up after they get deleted
//  i.e. nodes try to poke them on deletion even if they're gone
//  see test for an example of a crash

// This is a safer version, which cleans up after itself
// Use observe() to listen to object, remove() to stop listening
// Override objectDeleted() for special behavior

class OSGObserver {
public:
	OSGObserver();
	virtual ~OSGObserver();

	void observe(osg::Referenced *node);
	void remove(osg::Referenced *node);

	const std::set<osg::Referenced*> &observed();

	virtual void objectDeleted(void *obj);

private:
	class InternalObserver : public osg::Observer {
	public:
		InternalObserver(OSGObserver *boss);
		void objectDeleted(void *obj) override;
	private:
		OSGObserver *m_boss;
	};

private:
	InternalObserver m_observer;
	std::set<osg::Referenced*> m_observed;
};

#endif
