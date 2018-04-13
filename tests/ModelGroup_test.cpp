#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include <vector>
#include "ModelGroup.h"

struct NameYearTuple {
	std::string name;
	int begin;
	int end;
};

class ModelGroup_test : public QObject {
	Q_OBJECT
private slots:

void timeRegexTest() {

	std::vector<NameYearTuple> list = {
		{ "T:g466 -1450 -1349", -1450, -1349},
		{ "T:III -1369 -1349", -1369, -1349 },
		{ "T:flagstaff_20 -1369 -1349", -1369, -1349 },
		{ "T:01-middlekingdom -1523 -1466", -1523, -1466 },
		{ "T:02-Hatshepsut -1465 -1349", -1465, -1349 }
	};

	for (auto &tuple : list) {
		int begin = 0;
		int end = 0;
		ModelGroup::nodeTimeInName(tuple.name, &begin, &end);
		QCOMPARE(begin, tuple.begin);
		QCOMPARE(end, tuple.end);
	}
}
};

QTEST_MAIN(ModelGroup_test)
#include "ModelGroup_test.moc"