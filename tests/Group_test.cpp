#include <QTest>
#include <QDebug>
#include <set>
#include <QSignalSpy>
#include <QVariant>
#include <stdio.h>
#include <QUndoStack>
#include <QUndoCommand>
#include <QMetaType>
#include "GroupTemplate.h"
#include "GroupCommands.h"

struct Foo {
	int val;
	std::string name;
};

class Group_test : public QObject {
	Q_OBJECT
private:
	QUndoStack stack;

	void printGroup(TGroup<Foo> *group) {
		for (size_t i = 0; i < group->size(); i++) {
			qDebug() << i << group->child(i)->name.c_str();
		}
	}

	template<class T>
	void GROUPCOMPARE(TGroup<T> *group, std::vector<std::shared_ptr<Foo>> target) {
		QCOMPARE(group->size(), target.size());
		for (size_t i = 0; i < group->size(); i++) {
			if (i >= target.size()) return;
			QCOMPARE(group->childShared(i), target[i]);
		}
	}
	
	std::vector<Foo*> makeNodes(size_t count) {
		std::vector<Foo*> n;
		for (size_t i = 0; i < count; i++) {
			Foo* node = new Foo;
			node->name = std::to_string(i);
			node->val = i;
			n.push_back(node);
		}
		return n;
	}
	std::vector<std::shared_ptr<Foo>> makeShared(int count) {
		std::vector<std::shared_ptr<Foo>> n;
		for (int i = 0; i < count; i++) {
			Foo* node = new Foo;
			node->name = std::to_string(i);
			node->val = i;
			n.push_back(std::shared_ptr<Foo>(node));
		}
		return n;
	}
	//std::vector<std::shared_ptr<Foo>> makeRefs(const std::vector<Foo*> &nodes) {
	//	std::vector<std::shared_ptr<Foo>> n;
	//	for(auto node : nodes) {
	//		n.push_back(std::shared_ptr<Foo>(node));
	//	}
	//	return n;
	//}
	//get makeRefs(const std::vector<Foo*> &nodes) {
	//	std::vector<std::shared_ptr<Foo>> n;
	//	for (auto node : nodes) {
	//		n.push_back(std::shared_ptr<Foo>(node));
	//	}
	//	return n;
	//}

private slots:

	void sharedTest() {
		Foo *f = new Foo;
		std::shared_ptr<Foo> s1 = std::shared_ptr<Foo>(f);
		std::shared_ptr<Foo> s2 = s1;
	}

	void iterate() {
		TGroup<Foo> group;
		auto n = makeShared(4);
		group.insert(0, n);

		for (auto &shared : group) {
			qDebug() << shared->val;
		}
	}

	void move() {
		TGroup<Foo> group;
		auto n = makeShared(4);
		group.insert(0, n);

		// swap 0 to 1
		group.move({ {0,1} });
		GROUPCOMPARE(&group, {n[1], n[0], n[2], n[3]});

		// swap 2,3 to 1,2
		group.move({ {2,1},{3,2} });
		GROUPCOMPARE(&group, { n[1], n[2], n[3], n[0] });
	}
	void append() {
		TGroup<Foo> g;
		auto n = makeShared(2);
		auto x = std::shared_ptr<Foo>(new Foo);
		g.insert(0, n);
		g.append(x);
		GROUPCOMPARE(&g, { n[0], n[1], x });
		QCOMPARE(std::string("mee"), std::string("mee"));
		QCOMPARE(4u, (size_t)4);
	}
	void insert() {
		TGroup<Foo> g;
		auto n = makeShared(4);

		g.insert(0, {n[0], n[1]});
		GROUPCOMPARE(&g, { n[0], n[1] });

		g.insert(0, {n[2], n[3]});
		GROUPCOMPARE(&g, { n[2], n[3], n[0], n[1] });
	}
	void remove() {
		TGroup<Foo> g;
		auto n = makeShared(4);
		g.insert(0, n);

		g.remove(1, 2);
		GROUPCOMPARE(&g, { n[0], n[3] });
	}
	void insertMulti() {
		TGroup<Foo> g;
		auto n = makeShared(6);

		g.insertMulti({
			{ 0, n[0] },
			{ 1, n[1] },
			{ 2, n[2] }});
		GROUPCOMPARE(&g, { n[0], n[1], n[2] });

		g.insertMulti({
			{ 0, n[3] },
			{ 1, n[4] },
			{ 4, n[5] }});

		GROUPCOMPARE(&g, { n[3], n[4], n[0], n[1], n[5], n[2] });
	}

	void removeMulti() {
		TGroup<Foo> g;
		auto n = makeShared(6);
		g.insert(0, n);

		g.removeMulti({0});
		GROUPCOMPARE(&g, { n[1], n[2], n[3], n[4], n[5]});

		g.removeMulti({ 4 });
		GROUPCOMPARE(&g, { n[1], n[2], n[3], n[4]});

		g.removeMulti({ 0, 2 });
		GROUPCOMPARE(&g, { n[2], n[4] });
	}

	//void insertMultiP() {
	//	TGroup<Foo> g;
	//	auto n = makeShared(6);

	//	g.insertMultiP(std::set<std::shared_ptr<Foo>>(n.begin() + 1, n.begin() + 4));
	//	printGroup(&g);
	//	GROUPCOMPARE(&g, { n[1], n[2], n[3] });
	//}
	//void removeMultiP() {
	//	TGroup<Foo> g;
	//	auto n = makeShared(6);
	//	g.insert(0, n);

	//	g.removeMultiP({n[3], n[1], n[0]});
	//	GROUPCOMPARE(&g, { n[2], n[4], n[5] });
	//}

	void addNodeCommand() {
		QUndoStack stack;
		TGroup<Foo> g;
		auto n = makeShared(2);
		g.insert(0, n);

		auto x = std::make_shared<Foo>();
		x->name = "newguy";
		x->val = 100;
		stack.push(new AddNodeCommand<Foo>(&g, x));
		GROUPCOMPARE(&g, { n[0], n[1], x });

		stack.undo();
		GROUPCOMPARE(&g, { n[0], n[1] });

		stack.push(new AddNodeCommand<Foo>(&g, x, 1));
		printGroup(&g);
		GROUPCOMPARE(&g, { n[0], x, n[1] });
	}

	void moveCommand() {
		QUndoStack stack;
		TGroup<Foo> g;
		auto n = makeShared(4);
		g.insert(0, n);

		stack.push(new MoveNodesCommand<Foo>(&g, { {1, 2}, {2, 1} }));
		GROUPCOMPARE(&g, { n[0], n[2], n[1], n[3] });

		stack.undo();
		GROUPCOMPARE(&g, { n[0], n[1], n[2], n[3] });
	}

	void removeMultiCommand() {
		QUndoStack stack;
		TGroup<Foo> g;
		auto n = makeShared(4);
		g.insert(0, n);

		stack.push(new RemoveMultiCommand<Foo>(&g, { 1, 3 }));
		GROUPCOMPARE(&g, { n[0], n[2] });

		stack.undo();
		GROUPCOMPARE(&g, { n[0], n[1], n[2], n[3] });
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

};

QTEST_MAIN(Group_test)
#include "Group_test.moc"