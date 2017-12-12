#include <QTest>
#include <QDebug>
#include <set>
#include <QSignalSpy>
#include <QVariant>
#include "GroupProxy.h"
#include <stdio.h>
class GroupProxy_test : public QObject {
	Q_OBJECT
private:
	osg::ref_ptr<Group> group;
	osg::ref_ptr<GroupProxy> proxy;
	osg::ref_ptr<osg::Node> node0;
	osg::ref_ptr<osg::Node> node1;
	osg::ref_ptr<osg::Node> node2;
	osg::ref_ptr<osg::Node> node3;
	void reset() {
		group = new Group;
		node0 = new osg::Node;
		node1 = new osg::Node;
		node2 = new osg::Node;
		node3 = new osg::Node;
		node0->setName("node0");
		node1->setName("node1");
		node2->setName("node2");
		node3->setName("node3");
	}
	void debugGroup(Group *group) {
		qDebug() << group->getNumChildren();
		for (unsigned int i = 0; i < group->getNumChildren(); i++) {
			qDebug() << i << group->child(i) << QString::fromStdString(group->child(i)->getName());
		}
	}
private slots:
	void basicTest() {
		reset();
		group->addChild(node0);
		group->addChild(node1);

		proxy = new GroupProxy(group);

		// basic
		proxy->setMapToBase({ 0,1 });
		QCOMPARE(proxy->child(0)->getName(), "node0");
		QCOMPARE(proxy->child(1)->getName(), "node1");
		QCOMPARE(proxy->getNumChildren(), 2);
	}

	void emptyGroupTest() {
		reset();
		proxy = new GroupProxy(group);
		proxy->setMapToBase({});

		// has to be 0
		QCOMPARE(proxy->getNumChildren(), 0);

		group->addChild(node0);

		// still 0 since no mapping
		QCOMPARE(proxy->getNumChildren(), 0);
	}

	void reverseMapping() {
		reset();
		group->addChild(node0);
		group->addChild(node1);
		proxy = new GroupProxy(group);

		proxy->setMapToBase({ 1,0 });
		QCOMPARE(proxy->child(0)->getName(), "node1");
		QCOMPARE(proxy->child(1)->getName(), "node0");
		QCOMPARE(proxy->getNumChildren(), 2);
	}
	
	void insertConsistency() {
		reset();
		group->addChild(node0);
		proxy = new GroupProxy(group);
		proxy->setMapToBase({ 0 });

		group->insertChild(0, node1);
		QCOMPARE(proxy->getNumChildren(), 1);
		QCOMPARE(proxy->child(0)->getName(), "node0");
	}
	void moveConsistency() {
		reset();
		group->addChild(node0);
		group->addChild(node1);
		proxy = new GroupProxy(group);
		proxy->setMapToBase({ 0 });

		group->move({ { 0, 1 }, { 1, 0 } });
		QCOMPARE(proxy->getNumChildren(), 1);
		QCOMPARE(proxy->child(0)->getName(), "node0");
	}

	void deleteConsistency() {
		reset();
		group->addChild(node0);
		group->addChild(node1);
		proxy = new GroupProxy(group);
		proxy->setMapToBase({ 1 });
		qDebug() << "before delete";
		debugGroup(proxy);

		group->removeChild(0, 1);
		qDebug() << "after delete";
		debugGroup(proxy);
		QCOMPARE(proxy->getNumChildren(), 1);
		QCOMPARE(proxy->child(0)->getName(), "node1");

		// delete to nothingness
		debugGroup(group);
		debugGroup(proxy);
		group->removeChild(0, 1);
		debugGroup(group);
		debugGroup(proxy);
		QCOMPARE(proxy->getNumChildren(), 0);
	}
};

QTEST_MAIN(GroupProxy_test)
#include "GroupProxy_test.moc"