#include <QTest>
#include <QDebug>
#include "VecUtil.h"
#include <string>
#include <vector>
#include <iostream>
#include <cstddef>

struct NameYearTuple {
	std::string name;
	int begin;
	int end;
};

class VecUtil_test : public QObject {
	Q_OBJECT
private slots:
	void fixIndicesTest() {
		std::vector<size_t> mini, mini_ans, mini_result;
		std::set<size_t> mini_ins, mini_rem;
		mini = { 0, 1, 2, 3 };
		mini_ins = { 0, 2 };
		mini_ans = { 1, 3, 4, 5 };
		mini_result = VecUtil::fixIndicesAfterInsert(mini, mini_ins);
		QCOMPARE(mini_result, mini_ans);

		mini = { 0, 1 };
		mini_ins = { 0, 1 };
		mini_ans = { 2, 3 };
		mini_result = VecUtil::fixIndicesAfterInsert(mini, mini_ins);
		QCOMPARE(mini_result, mini_ans);

		// removals
		mini = { 1, 3, 4, 5 };
		mini_rem = { 0, 2 };
		mini_ans = { 0, 1, 2, 3 };
		mini_result = VecUtil::fixIndicesAfterRemove(mini, mini_rem);
		//qDebug() << "after remove";
		//for (int i : mini_result) {
		//	qDebug() << i;
		//}
		QCOMPARE(mini_result, mini_ans);

		mini = { 0, 1, 2 };
		mini_rem = { 0, 2 };

		mini_result = VecUtil::fixIndicesAfterRemove(mini, mini_rem);

		std::vector<size_t> ptrs;
		std::vector<std::pair<size_t, size_t>> moves;
		std::vector<size_t> res, res_ex;

		// a b c d e f
		// e c a b d f
		ptrs = {0, 1, 2, 3, 4, 5};
		moves = { {2, 1}, {4, 0} };
		res_ex = { 2, 3, 1, 4, 0, 5 };
		res = VecUtil::fixIndicesAfterMove(ptrs, moves);
		QCOMPARE(res, res_ex);

		// before: a, b, c
		// after:  b, c, a
		// ptrs:   2, 0, 1
		ptrs = { 0, 1, 2 };
		moves = { { 0, 2 } };
		res_ex = { 2, 0, 1 };
		res = VecUtil::fixIndicesAfterMove(ptrs, moves);
		QCOMPARE(res, res_ex);

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
		rem_ex = { 1, 4 }; // 4, 1, 2
		ins_ex = { 0, 1 }; // 3, 6, 4, 1, 2
		moves_ex = { {2, 3}, { 3, 2 } }; // 3, 6, 1, 4, 2
		VecUtil::removalsInsertionsMoves(before, after, &rem, &ins, &moves);
		QCOMPARE(rem, rem_ex);
		QCOMPARE(ins, ins_ex);
		QCOMPARE(moves, moves_ex);

		before = { 5, 9, 3, 1 };
		after = { 6, 5 };
		rem_ex = { 1, 2, 3 };
		ins_ex = { 0 };
		moves_ex = { };
		VecUtil::removalsInsertionsMoves(before, after, &rem, &ins, &moves);
		QCOMPARE(rem, rem_ex);
		QCOMPARE(ins, ins_ex);
		QCOMPARE(moves, moves_ex);

		before = { 0, 1 };
		after = { 1 };
		rem_ex = { 0 };
		ins_ex = { };
		moves_ex = { };
		VecUtil::removalsInsertionsMoves(before, after, &rem, &ins, &moves);
		//qDebug() << "removals" << rem.size();
		//qDebug() << "ins" << ins.size();
		//qDebug() << "move" << moves.size();
		//for (auto p : moves) {
		//	qDebug() << "m" << p.first << p.second;
		//}
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

};

QTEST_MAIN(VecUtil_test)
#include "VecUtil_test.moc"