#include <QTest>
#include <QDebug>
#include <set>
#include <QSignalSpy>
#include <QVariant>
#include "Group.h"
#include <stdio.h>
#include <QUndoStack>
#include <QUndoCommand>
#include <QMetaType>
class Group_test : public QObject {
	Q_OBJECT
private:
	osg::ref_ptr<Group> group;
	osg::ref_ptr<osg::Node> node0;
	osg::ref_ptr<osg::Node> node1;
	osg::ref_ptr<osg::Node> node2;
	osg::ref_ptr<osg::Node> node3;
	QUndoStack stack;
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
		stack.clear();
	}

	void GROUPCOMPARE(Group *group, std::vector<osg::Node*> target) {
		QCOMPARE(group->getNumChildren(), target.size());
		for (unsigned int i = 0; i < group->getNumChildren(); i++) {
			if (i >= target.size()) return;
			QCOMPARE(group->child(i), target[i]);
		}
	}
private slots:
	void addNodeCommand() {
		reset();
		group->addChild(node0);
		QSignalSpy newSpy(group, &Group::sNew);
		QSignalSpy deleteSpy(group, &Group::sDelete);
		stack.push(new Group::AddNodeCommand(group, node1));
		QCOMPARE(newSpy.size(), 1);

		GROUPCOMPARE(group, { node0, node1 });
		stack.undo();
		GROUPCOMPARE(group, { node0 });
		QCOMPARE(deleteSpy.size(), 1);
	}
	void insertNodeCommand() {
		reset();
		group->addChild(node0);

		stack.push(new Group::AddNodeCommand(group, node1, 0));
		GROUPCOMPARE(group, { node1, node0 });
		stack.undo();
		GROUPCOMPARE(group, { node0 });
	}
	void deleteNodeCommand() {
		reset();
		group->addChild(node0);
		group->addChild(node1);
		QSignalSpy newSpy(group, &Group::sNew);
		QSignalSpy deleteSpy(group, &Group::sDelete);
		stack.push(new Group::DeleteNodeCommand(group, 0));
		QCOMPARE(deleteSpy.size(), 1);

		GROUPCOMPARE(group, { node1 });
		stack.undo();
		GROUPCOMPARE(group, { node0, node1 });
		QCOMPARE(newSpy.size(), 1);
	}
	void move() {
		reset();
		group->addChild(node0);
		group->addChild(node1);
		group->addChild(node2);
		group->addChild(node3);
		qRegisterMetaType<std::vector<std::pair<int, int>>>("MoveList");
		QSignalSpy spy(group, &Group::sItemsMoved);

		// swap 0 to 1
		qInfo() << "swap 1";
		group->move({ {0,1} });
		GROUPCOMPARE(group, {node1, node0, node2, node3});
		QCOMPARE(spy.size(), 1);

		qInfo() << "swap 0, 2, 3";
		// swap 2,3 to 1,2
		group->move({ {2,1},{3,2} });
		GROUPCOMPARE(group, {node1, node2, node3, node0});
	}
	void removeChildren() {
		reset();
		group->addChild(node0);
		group->addChild(node1);
		group->addChild(node2);
		QSignalSpy spy(group, &Group::sDelete);
		group->removeChildren(0, 2);
		GROUPCOMPARE(group, { node2 });
		QCOMPARE(spy.size(), 2);
	}
	void removeChildren2() {
		reset();
		group->addChild(node0);
		group->addChild(node1);
		QSignalSpy spy(group, &Group::sDelete);
		group->removeChildren(0, 2);
		GROUPCOMPARE(group, {});
		QCOMPARE(spy.size(), 2);
	}
};

QTEST_MAIN(Group_test)
#include "Group_test.moc"