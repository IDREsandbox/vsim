#include <QTest>
#include <QDebug>
#include <set>
#include <QSignalSpy>
#include <QVariant>
#include "Selection.h"
#include <stdio.h>
class Selection_test : public QObject {
	Q_OBJECT
private slots:
	void construction() {
		Selection s;
		QCOMPARE(s.getLastSelected(), -1);
		QCOMPARE(s.getSelection(), std::set<int>());
	}
	void setTest() {
		Selection s;
		QSignalSpy addSpy(&s, &Selection::sAdded);
		QSignalSpy removeSpy(&s, &Selection::sRemoved);
		QSignalSpy changeSpy(&s, &Selection::sChanged);

		s.set({ 1, 2 }, 1);
		QCOMPARE(addSpy.count(), 2);
		QCOMPARE(addSpy.takeFirst().at(0).toInt(), 1);
		QCOMPARE(addSpy.takeFirst().at(0).toInt(), 2);
		QCOMPARE(changeSpy.count(), 1);
		changeSpy.clear();

		s.set({3}, 3);
		QCOMPARE(addSpy.count(), 1);
		QCOMPARE(addSpy.takeFirst().at(0).toInt(), 3);
		std::set<int> removed;
		QCOMPARE(removeSpy.count(), 2);
		removed.insert(removeSpy.takeFirst().at(0).toInt());
		removed.insert(removeSpy.takeFirst().at(0).toInt());
		QCOMPARE(removed, std::set<int>({ 1, 2 }));
		QCOMPARE(changeSpy.count(), 1);
	}
	void clearTest() {
		Selection s;
		s.set({ 1,4,5 }, 1);
		QSignalSpy addSpy(&s, &Selection::sAdded);
		QSignalSpy removeSpy(&s, &Selection::sRemoved);
		QSignalSpy changeSpy(&s, &Selection::sChanged);
		s.clear();

		QCOMPARE(addSpy.count(), 0);
		QCOMPARE(removeSpy.count(), 3);
		QCOMPARE(std::set<int>({}), s.getSelection());
	}
	void addRemoveTest() {
		Selection s;
		s.add(9);
		QCOMPARE(s.getSelection(), std::set<int>({ 9 }));
		QCOMPARE(s.getLastSelected(), 9);
		s.add(4);
		s.add(3);
		QCOMPARE(s.getSelection(), std::set<int>({3, 4, 9}));
		QCOMPARE(s.getLastSelected(), 3);		
		s.remove(9);
		QCOMPARE(s.getSelection(), std::set<int>({ 3, 4 }));
		QCOMPARE(s.getLastSelected(), 4);
	}
	void containsTest() {
		Selection s;
		std::set<int> list = { 1, 9, 11, 14 };
		s.set(list, -1);

		QCOMPARE(s.contains(1), true);
		QCOMPARE(s.contains(11), true);
		QCOMPARE(s.contains(0), false);
		QCOMPARE(s.contains(17), false);
	}
	void shiftSelectTest() {
		Selection s;
		s.set({ 1, 12 }, 12);

		s.shiftSelect(15);
		QCOMPARE(s.getSelection(), std::set<int>({ 1, 12, 13, 14, 15 }));

		s.set({ 1, 12 }, 12);
		s.shiftSelect(10);
		QCOMPARE(s.getSelection(), std::set<int>({ 1, 10, 11, 12 }));
	}
	void nextAfterDeleteTest() {
		std::set<int> list = {4,6};
		// test after
		QCOMPARE(Selection::nextAfterDelete(3, {0}), 0);
		QCOMPARE(Selection::nextSelectionAfterDelete(3, {0}), std::set<int>({0}));
		// test inbetween
		QCOMPARE(Selection::nextAfterDelete(7, {4,6}), 4);
		// test previous
		QCOMPARE(Selection::nextAfterDelete(5, {4}), 3);
		QCOMPARE(Selection::nextAfterDelete(5, {4}), {3});
		// test empty
		QCOMPARE(Selection::nextAfterDelete(3, {0,1,2}), -1);
		QCOMPARE(Selection::nextSelectionAfterDelete(3, {0,1,2}), std::set<int>());
	}

};

QTEST_MAIN(Selection_test)
#include "Selection_test.moc"