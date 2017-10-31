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
	void setSelectionTest() {	
		Selection s;
		QSignalSpy addSpy(&s, &Selection::sAdded);
		QSignalSpy removeSpy(&s, &Selection::sRemoved);
		QSignalSpy changeSpy(&s, &Selection::sChanged);

		s.setSelection({ 1, 2 }, 1);
		QCOMPARE(addSpy.count(), 2);
		QCOMPARE(addSpy.takeFirst().at(0).toInt(), 1);
		QCOMPARE(addSpy.takeFirst().at(0).toInt(), 2);
		QCOMPARE(changeSpy.count(), 1);
		changeSpy.clear();

		s.setSelection({3}, 3);
		QCOMPARE(addSpy.count(), 1);
		QCOMPARE(addSpy.takeFirst().at(0).toInt(), 3);
		std::set<int> removed;
		QCOMPARE(removeSpy.count(), 2);
		removed.insert(removeSpy.takeFirst().at(0).toInt());
		removed.insert(removeSpy.takeFirst().at(0).toInt());
		QCOMPARE(removed, std::set<int>({ 1, 2 }));
		QCOMPARE(changeSpy.count(), 1);
	}
	void clearSelectionTest() {
		Selection s;
		s.setSelection({ 1,4,5 }, 1);
		QSignalSpy addSpy(&s, &Selection::sAdded);
		QSignalSpy removeSpy(&s, &Selection::sRemoved);
		QSignalSpy changeSpy(&s, &Selection::sChanged);
		s.clearSelection();

		QCOMPARE(addSpy.count(), 0);
		QCOMPARE(removeSpy.count(), 3);
		QCOMPARE(std::set<int>({}), s.getSelection());
	}
	void addRemoveTest() {
		Selection s;
		s.addToSelection(9);
		QCOMPARE(s.getSelection(), std::set<int>({ 9 }));
		QCOMPARE(s.getLastSelected(), 9);
		s.addToSelection(4);
		s.addToSelection(3);
		QCOMPARE(s.getSelection(), std::set<int>({3, 4, 9}));
		QCOMPARE(s.getLastSelected(), 3);		
		s.removeFromSelection(9);
		QCOMPARE(s.getSelection(), std::set<int>({ 3, 4 }));
		QCOMPARE(s.getLastSelected(), 4);
	}
	void isSelectedTest() {
		Selection s;
		std::set<int> list = { 1, 9, 11, 14 };
		s.setSelection(list, -1);

		QCOMPARE(s.isSelected(1), true);
		QCOMPARE(s.isSelected(11), true);
		QCOMPARE(s.isSelected(0), false);
		QCOMPARE(s.isSelected(17), false);
	}
	void nextAfterDeleteTest() {
		std::set<int> list = {4,6};
		// test after
		QCOMPARE(Selection::nextAfterDelete(3, {0}), 0);
		// test inbetween
		QCOMPARE(Selection::nextAfterDelete(7, {4,6}), 4);
		// test previous
		QCOMPARE(Selection::nextAfterDelete(5, {4}), 3);
		// test empty
		QCOMPARE(Selection::nextAfterDelete(3, {0,1,2}), -1);
	}

};

QTEST_MAIN(Selection_test)
#include "Selection_test.moc"