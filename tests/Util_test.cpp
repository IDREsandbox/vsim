#include <QTest>
//#include <QDebug>
#include "Util.h"

class Util_test : public QObject {
	Q_OBJECT
private slots:
	void fixIndicesTest() {
		std::vector<int> mini = { 0, 1, 2, 3 };
		std::set<int> mini_ins = { 0, 2 };
		std::vector<int> mini_ans = { 1, 3, 4, 5 };
		std::vector<int> mini_result = Util::fixIndices(mini, mini_ins);
		qDebug() << "result";
		for (int i : mini_result) {
			qDebug() << i;
		}
		QCOMPARE(mini_result, mini_ans);

		std::vector<int> mini2 = { 0, 1 };
		std::set<int> mini_ins2 = { 0, 1 };
		std::vector<int> mini_ans2 = { 2, 3 };
		std::vector<int> mini_result2 = Util::fixIndices(mini2, mini_ins2);
		qDebug() << "result";
		for (int i : mini_result2) {
			qDebug() << i;
		}
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
};

QTEST_MAIN(Util_test)
#include "Util_test.moc"