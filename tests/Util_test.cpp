#include <QTest>
#include <QDebug>
#include "Util.h"
#include <string>
#include <vector>
#include <iostream>

class Util_test : public QObject {
	Q_OBJECT
private slots:
	void fixIndicesTest() {
		std::vector<int> mini = { 0, 1, 2, 3 };
		std::set<int> mini_ins = { 0, 2 };
		std::vector<int> mini_ans = { 1, 3, 4, 5 };
		std::vector<int> mini_result = Util::fixIndices(mini, mini_ins);
		QCOMPARE(mini_result, mini_ans);

		std::vector<int> mini2 = { 0, 1 };
		std::set<int> mini_ins2 = { 0, 1 };
		std::vector<int> mini_ans2 = { 2, 3 };
		std::vector<int> mini_result2 = Util::fixIndices(mini2, mini_ins2);
		QCOMPARE(mini_result2, mini_ans2);


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
		//big_result = Util::fixIndices(big, big_ins);
		//qDebug() << "big 1/10 insert, map" << Util::toc();

		//// big, 1 insert
		//Util::tic();
		//big_result = Util::fixIndices(big, { 0 });
		//qDebug() << "big 1 insert, map" << Util::toc();

	}
	void multiInsertTest() {
		std::vector<std::string> strings = { "hey", "man", "sup" };
		Util::multiInsert(&strings, { {0, "what"}, {3, "kay"} });
		std::vector<std::string> expected = { "what", "hey", "man", "kay", "sup" };
		QCOMPARE(strings, expected);

		// out of range test
		strings = {};
		Util::multiInsert(&strings, { { 2, "lmao" } });
		expected = { "lmao" };
		QCOMPARE(strings, expected);

		// out of order
		strings = {};
		Util::multiInsert(&strings, { { 2, "there"}, { 1, "who"} });
		expected = { "there", "who" };
		QCOMPARE(strings, expected);
	}
	void multiRemoveTest() {
		std::vector<std::string> strings = { "hey", "man", "sup" };
		Util::multiRemove(&strings, { 0, 1 });
		std::vector<std::string> expected = { "sup" };
		QCOMPARE(strings, expected);

		strings = { "hey", "man", "sup" };
		Util::multiRemove(&strings, { 2 });
		expected = { "hey", "man" };
		QCOMPARE(strings, expected);
	}
	void multiMove() {
		std::vector<std::string> strings = { "a", "b", "c", "d" };
		std::vector<std::string> expected = { "b", "c", "a", "d" };
		Util::multiMove(&strings, { {0, 2}, {1, 0}, {2, 1} });

		strings = { "a", "b", "c", "d" };
		expected = { "d", "a", "b", "c" };
		Util::multiMove(&strings, { { 3, 0 } });
		for (auto s : strings) {
			qDebug() << "r" << s.c_str();
		}
		QCOMPARE(strings, expected);
	}
	void clumpify() {
		std::vector<size_t> indices = {1,2,3,5,6,7,8,9};
		std::vector<std::pair<size_t, size_t>> expected = {{1,3}, {5,9}};
		std::vector<std::pair<size_t, size_t>> result = Util::clumpify(indices);
		QCOMPARE(result, expected);

		indices = {};
		expected = {};
		result = Util::clumpify(indices);
		QCOMPARE(result, expected);

		indices = {1};
		expected = {{1,1}};
		result = Util::clumpify(indices);
		QCOMPARE(result, expected);

		indices = {1,700};
		expected = {{1,1}, {700,700}};
		result = Util::clumpify(indices);
		QCOMPARE(result, expected);
	}
};

QTEST_MAIN(Util_test)
#include "Util_test.moc"