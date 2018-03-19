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
	QUndoStack stack;

	void GROUPCOMPARE(Group *group, std::vector<osg::Node*> target) {
		QCOMPARE(group->getNumChildren(), target.size());
		for (unsigned int i = 0; i < group->getNumChildren(); i++) {
			if (i >= target.size()) return;
			QCOMPARE(group->child(i), target[i]);
		}
	}
	
	std::vector<osg::Node*> makeNodes(size_t count) {
		std::vector<osg::Node*> n;
		for (size_t i = 0; i < count; i++) {
			osg::Node* node = new osg::Node;
			node->setName(std::to_string(i));
			n.push_back(node);
		}
		return n;
	}
	std::vector<osg::ref_ptr<osg::Node>> makeRefs(const std::vector<osg::Node*> &nodes) {
		std::vector<osg::ref_ptr<osg::Node>> n;
		for(auto node : nodes) {
			n.push_back(osg::ref_ptr<osg::Node>(node));
		}
		return n;
	}
private slots:
	//void dumbCommand() {
	//	reset();
	//	group->addChild(node0);
	//	group->addChild(node1);
	//}
	//void removeSignals() {
	//	reset();
	//	group->addChild(node0);
	//	group->addChild(node1);
	//	group->addChild(node2);
	//	group->addChild(node3);

	//	QSignalSpy deleteSpy(group, &Group::sDelete);
	//	group->removeChildren(1, 2);
	//	QCOMPARE(deleteSpy.size(), 2);
	//	// reverse order deletion signals
	//	QCOMPARE(deleteSpy.takeFirst().at(0).toInt(), 2);
	//	QCOMPARE(deleteSpy.takeFirst().at(0).toInt(), 1);
	//}
	//void addNodeCommand() {
	//	reset();
	//	group->addChild(node0);
	//	QSignalSpy newSpy(group, &Group::sNew);
	//	QSignalSpy deleteSpy(group, &Group::sDelete);
	//	stack.push(new Group::AddNodeCommand(group, node1));
	//	QCOMPARE(newSpy.size(), 1);

	//	GROUPCOMPARE(group, { node0, node1 });
	//	stack.undo();
	//	GROUPCOMPARE(group, { node0 });
	//	QCOMPARE(deleteSpy.size(), 1);
	//}
	//void insertNodeCommand() {
	//	reset();
	//	group->addChild(node0);

	//	stack.push(new Group::AddNodeCommand(group, node1, 0));
	//	GROUPCOMPARE(group, { node1, node0 });
	//	stack.undo();
	//	GROUPCOMPARE(group, { node0 });
	//}
	//void deleteNodeCommand() {
	//	reset();
	//	group->addChild(node0);
	//	group->addChild(node1);
	//	QSignalSpy newSpy(group, &Group::sNew);
	//	QSignalSpy deleteSpy(group, &Group::sDelete);
	//	stack.push(new Group::DeleteNodeCommand(group, 0));
	//	QCOMPARE(deleteSpy.size(), 1);
	//	QCOMPARE(deleteSpy.takeFirst().at(0).toInt(), 0);

	//	GROUPCOMPARE(group, { node1 });
	//	stack.undo();
	//	GROUPCOMPARE(group, { node0, node1 });
	//	QCOMPARE(newSpy.size(), 1);
	//}
	void move() {
		osg::ref_ptr<Group> group;
		osg::ref_ptr<osg::Node> node0;
		osg::ref_ptr<osg::Node> node1;
		osg::ref_ptr<osg::Node> node2;
		osg::ref_ptr<osg::Node> node3;
		group = new Group;
		node0 = new osg::Node;
		node1 = new osg::Node;
		node2 = new osg::Node;
		node3 = new osg::Node;
		group->addChild(node0);
		group->addChild(node1);
		group->addChild(node2);
		group->addChild(node3);
		node0->setName("0");
		node1->setName("1");
		node2->setName("2");
		node3->setName("3");
		//qRegisterMetaType<std::vector<std::pair<size_t, size_t>>>("MoveList");
		//QSignalSpy spy(group, &Group::sItemsMoved);

		// swap 0 to 1
		qInfo() << "swap 1";
		group->move({ {0,1} });
		GROUPCOMPARE(group, {node1, node0, node2, node3});
		//qDebug() << "after swap";
		//for (uint i = 0; i < group->getNumChildren(); i++) {
		//	qDebug() << group->child(i)->getName().c_str();
		//}
		//QCOMPARE(spy.size(), 1);

		qInfo() << "swap 0, 2, 3";
		// swap 2,3 to 1,2
		group->move({ {2,1},{3,2} });
		GROUPCOMPARE(group, {node1, node2, node3, node0});
	}
	//void removeChildren() {
	//	reset();
	//	group->addChild(node0);
	//	group->addChild(node1);
	//	group->addChild(node2);
	//	QSignalSpy spy(group, &Group::sDelete);
	//	group->removeChildren(0, 2);
	//	GROUPCOMPARE(group, { node2 });
	//	QCOMPARE(spy.size(), 2);
	//}
	//void removeChildren2() {
	//	reset();
	//	group->addChild(node0);
	//	group->addChild(node1);
	//	QSignalSpy spy(group, &Group::sDelete);
	//	group->removeChildren(0, 2);
	//	GROUPCOMPARE(group, {});
	//	QCOMPARE(spy.size(), 2);
	//}
	//void addChild() {
	//	reset();
	//	QSignalSpy spy(group, &Group::sNew);
	//	group->addChild(node0);
	//	QCOMPARE(spy.size(), 1);
	//}
	void insert() {
		osg::ref_ptr<Group> g = new Group;
		auto n = makeNodes(6);
		auto ref = makeRefs(n);

		g->insert(0, {n[0], n[1]});
		GROUPCOMPARE(g, { n[0], n[1] });

		g->insert(0, {n[2], n[3]});
		GROUPCOMPARE(g, { n[2], n[3], n[0], n[1] });
	}
	void remove() {

	}
	void insertChildrenSet() {
		osg::ref_ptr<Group> g = new Group;
		std::vector<osg::Node*> n;
		for (size_t i = 0; i < 6; i++) {
			osg::Node *node = new osg::Node;
			node->setName(std::to_string(i));
			n.push_back(node);
		}

		g->insertChildrenSet({
			{ 0, n[0] },
			{ 1, n[1] },
			{ 2, n[2] }});
		GROUPCOMPARE(g, { n[0], n[1], n[2] });

		g->insertChildrenSet({
			{ 0, n[3] },
			{ 1, n[4] },
			{ 4, n[5] }});
		//for (size_t i = 0; i < g->getNumChildren(); i++) {
		//	// did these guys get deleted?
		//	qDebug() << g->child(i) << g->child(i)->getName().c_str();
		//}
		GROUPCOMPARE(g, { n[3], n[4], n[0], n[1], n[5], n[2] });
	}
	void removeChildrenSet() {
		osg::ref_ptr<Group> g = new Group;
		std::vector<osg::Node*> n;
		for (size_t i = 0; i < 6; i++) {
			osg::Node *node = new osg::Node;
			node->setName(std::to_string(i));
			n.push_back(node);
			g->addChild(node);
		}

		g->removeChildrenSet({0});
		GROUPCOMPARE(g, { n[1], n[2], n[3], n[4], n[5]});

		g->removeChildrenSet({ 4 });
		GROUPCOMPARE(g, { n[1], n[2], n[3], n[4]});

		g->removeChildrenSet({ 1, 2 });
		GROUPCOMPARE(g, { n[1], n[4] });
	}
	//void removeChildrenP() {
	//	osg::ref_ptr<Group> g = new Group;
	//	std::vector<osg::Node*> n;
	//	for (size_t i = 0; i < 6; i++) {
	//		osg::Node *node = new osg::Node;
	//		node->setName(std::to_string(i));
	//		n.push_back(node);
	//		g->addChild(node);
	//	}

	//	g->removeChildrenP({n[3], n[1], n[0]});
	//	GROUPCOMPARE(g, { n[2], n[4], n[5] });
	//}
};

QTEST_MAIN(Group_test)
#include "Group_test.moc"