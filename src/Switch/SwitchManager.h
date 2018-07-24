#ifndef SWITCHMANAGER_H
#define SWITCHMANAGER_H

#include <unordered_map>
#include <QObject>
#include <osg/Switch>
#include <osgSim/MultiSwitch>
#include <osg/NodeVisitor>
#include <osg/observer_ptr>

class SwitchListModel;
class SwitchModel;

class SwitchManager : public QObject {
	Q_OBJECT;
public:
	SwitchManager(QObject *parent = nullptr);

	void clear();

	// adds switch to Qt models and gui
	// there are no live signals so name changes aren't supported
	void addSwitch(osg::Switch *sw);
	//void addMultiSwitch(osg::MultiSwitch *sw);
	void addNode(osg::Node *node);
	void addNodeRecursive(osg::Node *node);

	void removeNode(osg::Node *node);
	// void removeNodeRecursive(osg::Node *node);

	SwitchListModel *listModel();
	SwitchModel *switchModel();


private:
	class SwitchInitVisitor : public osg::NodeVisitor {
	public:
		SwitchInitVisitor(SwitchManager *boss);
		void apply(osg::Switch &node) override;
		//void apply(osg::Group &group) override;
		//void apply(osg::Node &node) override;
	private:
		SwitchManager *m_manager;
	};

	class SwitchObserver : public osg::Observer {
	public:
		SwitchObserver(SwitchManager *boss);
		void objectDeleted(void *obj) override;
	private:
		SwitchManager *m_manager;
	};

	//struct SwitchData {
	//	osg::Switch *m_switch;
	//	SwitchObserver m_observer;
	//};

private:
	//std::unordered_map<osg::Node*, SwitchData> m_switches;

	SwitchObserver m_observer;
	SwitchListModel *m_list_model;
	SwitchModel *m_switch_model;
};

#endif
