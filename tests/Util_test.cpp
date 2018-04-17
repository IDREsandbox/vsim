#include <QTest>
#include <QDebug>
#include "Util.h"
#include "VecUtil.h"
#include <string>
#include <vector>
#include <iostream>

class Util_test : public QObject {
	Q_OBJECT
private slots:
	void mxdxyyDateTest() {
		QDate date;
		bool hit;
		hit = Util::mxdxyyToQDate("1/2/94", &date);
		QVERIFY(hit);
		QCOMPARE(date, QDate(1994, 1, 2));

		hit = Util::mxdxyyToQDate("111/2/1994", &date);
		QVERIFY(!hit);

		hit = Util::mxdxyyToQDate("11/2/80", &date);
		QVERIFY(hit);
		QCOMPARE(date, QDate(1980, 11, 2));
		
		hit = Util::mxdxyyToQDate("12/30/79", &date);
		QVERIFY(hit);
		QCOMPARE(date, QDate(2079, 12, 30));
	}
	void fixIndicesTest() {
		std::vector<size_t> mini, mini_ans, mini_result;
		std::set<size_t> mini_ins, mini_rem;
		mini = { 0, 1, 2, 3 };
		mini_ins = { 0, 2 };
		mini_ans = { 1, 3, 4, 5 };
		mini_result = VecUtil::fixIndices(mini, mini_ins);
		QCOMPARE(mini_result, mini_ans);

		mini = { 0, 1 };
		mini_ins = { 0, 1 };
		mini_ans = { 2, 3 };
		mini_result = VecUtil::fixIndices(mini, mini_ins);
		QCOMPARE(mini_result, mini_ans);

		// removals
		mini = { 1, 3, 4, 5 };
		mini_rem = { 0, 2 };
		mini_ans = { 0, 1, 2, 3 };
		mini_result = VecUtil::fixIndicesRemove(mini, mini_rem);
		//qDebug() << "after remove";
		//for (int i : mini_result) {
		//	qDebug() << i;
		//}
		QCOMPARE(mini_result, mini_ans);
		//// speed tests for fun
		//// big, 1/10 insert
		//std::vector<int> big;
		//for (int i = 0; i < 50000; i++) {
		//	big.push_back(i);
		//}
		//std::set<int> big_ins;
		//for (int i = 0; i < 50000; i++) {
		//	if (i % 10 == 0) {
		//		big_ins.insert(i);
		//	}
		//}
		//std::vector<int> big_result;
		//Util::tic();
		//big_result = VecUtil::fixIndices(big, big_ins);
		//qDebug() << "big 1/10 insert, map" << Util::toc();

		//// big, 1 insert
		//Util::tic();
		//big_result = VecUtil::fixIndices(big, { 0 });
		//qDebug() << "big 1 insert, map" << Util::toc();

	}
	void multiInsertTest() {
		std::vector<std::string> strings = { "hey", "man", "sup" };
		VecUtil::multiInsert(&strings, { {0, "what"}, {3, "kay"} });
		std::vector<std::string> expected = { "what", "hey", "man", "kay", "sup" };
		QCOMPARE(strings, expected);

		// out of range test
		strings = {};
		VecUtil::multiInsert(&strings, { { 2, "lmao" } });
		expected = { "lmao" };
		QCOMPARE(strings, expected);

		// out of order
		strings = {};
		VecUtil::multiInsert(&strings, { { 2, "there"}, { 1, "who"} });
		expected = { "there", "who" };
		QCOMPARE(strings, expected);
	}
	void multiRemoveTest() {
		std::vector<std::string> strings = { "hey", "man", "sup" };
		VecUtil::multiRemove(&strings, { 0, 1 });
		std::vector<std::string> expected = { "sup" };
		QCOMPARE(strings, expected);

		strings = { "hey", "man", "sup" };
		VecUtil::multiRemove(&strings, { 2 });
		expected = { "hey", "man" };
		QCOMPARE(strings, expected);
	}
	void multiMove() {
		std::vector<std::string> strings = { "a", "b", "c", "d" };
		std::vector<std::string> expected = { "b", "c", "a", "d" };
		VecUtil::multiMove(&strings, { {0, 2}, {1, 0}, {2, 1} });

		strings = { "a", "b", "c", "d" };
		expected = { "d", "a", "b", "c" };
		VecUtil::multiMove(&strings, { { 3, 0 } });
		for (auto s : strings) {
			qDebug() << "r" << s.c_str();
		}
		QCOMPARE(strings, expected);
	}
	void removalsInsertionsMoves() {
		std::vector<size_t> before;
		std::vector<size_t> after;
		std::vector<size_t> rem;
		std::vector<size_t> ins;
		std::vector<std::pair<size_t, size_t>> moves;
		std::vector<size_t> rem_ex;
		std::vector<size_t> ins_ex;
		std::vector<std::pair<size_t, size_t>> moves_ex;

		before = { 4, 7, 1, 2, 5 };
		after = { 3, 6, 1, 4, 2 };
		rem_ex = { 1, 4 };
		ins_ex = { 0, 1 };
		moves_ex = { {0, 3}, {3, 4} };
		VecUtil::removalsInsertionsMoves(before, after, &rem, &ins, &moves);
		QCOMPARE(rem, rem_ex);
		QCOMPARE(ins, ins_ex);
		QCOMPARE(moves, moves_ex);
	}
	void clumpify() {
		std::vector<size_t> indices;
		std::vector<std::pair<size_t, size_t>> expected;
		std::vector<std::pair<size_t, size_t>> result;

		indices = {1,2,3,5,6,7,8,9};
		expected = {{1,3}, {5,9}};
		result = VecUtil::clumpify(indices);
		QCOMPARE(result, expected);

		indices = {};
		expected = {};
		result = VecUtil::clumpify(indices);
		QCOMPARE(result, expected);

		indices = {1};
		expected = {{1,1}};
		result = VecUtil::clumpify(indices);
		QCOMPARE(result, expected);

		indices = {1,700};
		expected = {{1,1}, {700,700}};
		result = VecUtil::clumpify(indices);
		QCOMPARE(result, expected);
	}
	void clumpify3() {
		std::vector<size_t> indices;
		std::vector<std::pair<size_t, size_t>> expected;
		std::vector<std::pair<size_t, size_t>> result;

		indices = { 1,2,3,5,6,7,8,9 };
		expected = { { 1,3 },{ 5,5 } };
		result = VecUtil::clumpify3(indices);
		QCOMPARE(result, expected);

		indices = {};
		expected = {};
		result = VecUtil::clumpify3(indices);
		QCOMPARE(result, expected);

		indices = { 1 };
		expected = { { 1,1 } };
		result = VecUtil::clumpify3(indices);
		QCOMPARE(result, expected);

		indices = { 1,700 };
		expected = { { 1,1 },{ 700,1 } };
		result = VecUtil::clumpify3(indices);
		QCOMPARE(result, expected);

		indices = { 6,7,8,9 };
		expected = { { 6,4 } };
		result = VecUtil::clumpify3(indices);
		QCOMPARE(result, expected);
	}
	void clumpify2() {
		std::vector<std::pair<size_t, char>> list;
		std::vector<std::pair<size_t, std::vector<char>>> expected;
		std::vector<std::pair<size_t, std::vector<char>>> result;

		list = {};
		expected = {};
		result = VecUtil::clumpify2(list);
		QCOMPARE(result, expected);

		list = {{9, 'a'}};
		expected = {{9, {'a'}}};
		result = VecUtil::clumpify2(list);
		QCOMPARE(result, expected);

		list = {{0, 'a'}, {2, 'b'}, {3, 'c'}, {4, 'e'}, {6, 'o'}};
		expected = {{0, {'a'}}, {2, {'b', 'c', 'e'}}, {6, {'o'}}};
		result = VecUtil::clumpify2(list);
		QCOMPARE(result, expected);
	}
	void spheresTest() {
		QCOMPARE(Util::spheresOverlap({ 0, 0, 0 }, 3.0f, { 4, 0, 0 }, 2.0f), true);
		QCOMPARE(Util::spheresOverlap({ 0, 0, 0 }, 3.0f, { 4, 0, 0 }, .999f), false);
		QCOMPARE(Util::spheresOverlap({ 0, 0, 0 }, 3.0f, { 4, 0, 0 }, 1.01f), true);
	}
	void matrixEqTest(){
		double d1[16] = {
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		};
		osg::Matrix m1(d1);
		double d2[16] = {
			0, 0, 0, 6.8,
			1.2, 0, 0, 0,
			0, 0, 4.5, 0,
			0, 0, 0, 0
		};
		osg::Matrix m2(d2);
		double d3[16] = {
			0, 0, 0, .0001,
			.0008, 0, 0, 0,
			0, -.0001, 0, 0,
			0, 0, 0, 0
		};
		osg::Matrix m3(d3);
		QVERIFY(Util::osgMatrixEq(m1, m1) == true);
		QVERIFY(Util::osgMatrixEq(m1, m2) == false);
		QVERIFY(Util::osgMatrixEq(m1, m3, .001) == true);
	}
};

QTEST_MAIN(Util_test)
#include "Util_test.moc"