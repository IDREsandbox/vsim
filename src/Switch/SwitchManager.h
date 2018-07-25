#ifndef SWITCHMANAGER_H
#define SWITCHMANAGER_H

#include <set>
#include <QObject>
#include <osg/Switch>
#include <osgSim/MultiSwitch>
#include <osg/NodeVisitor>
#include <osg/observer_ptr>
#include "Core/OSGObserver.h"

class SwitchListModel;
class OSGObserver;

class SwitchManager : public QObject {
	Q_OBJECT;
public:
	SwitchManager(QObject *parent = nullptr);

	void clear();

	// adds switch to Qt models and gui
	// there are no live signals so instant change propagation
	// isn't guaranteed
	void addNodeRecursive(osg::ref_ptr<osg::Node> node);

	// node shouldn't be invalid, deleted
	void removeNodeRecursive(osg::ref_ptr<osg::Node> node);

	// node can be null, invalid, already deleted
	void removeNode(osg::Node *node);

	SwitchListModel *listModel();

private:
	void track(osg::Node *node);

	void addSwitch(osg::Switch *sw);
	void addMultiSwitch(osgSim::MultiSwitch *ms);

	void removeNodeInternal(osg::Node *node, bool valid);

private:
	class SwitchInitVisitor : public osg::NodeVisitor {
	public:
		SwitchInitVisitor(SwitchManager *boss);
		void apply(osg::Switch &node) override;
		void apply(osg::Group &group) override;
	private:
		SwitchManager *m_manager;
	};

	class SwitchRemoveVisitor : public osg::NodeVisitor {
	public:
		SwitchRemoveVisitor(SwitchManager *boss);
		void apply(osg::Switch &node) override;
		void apply(osg::Group &group) override;
	private:
		SwitchManager *m_manager;
	};

	class SwitchObserver : public OSGObserver {
	public:
		SwitchObserver(SwitchManager *boss);
		void objectDeleted(void *obj) override;
	private:
		SwitchManager *m_manager;
	};

private:
	SwitchObserver m_observer;
	SwitchListModel *m_list_model;
};

#endif
