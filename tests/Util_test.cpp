#include <QTest>
//#include <QDebug>
#include "Util.h"

class Util_test : public QObject {
	Q_OBJECT
private slots:
	void test1() {
		QVERIFY(false);
	}
	void test2() {
		QString str = "hello";
		QCOMPARE(str.toUpper(), QString("HELLO"));
		qDebug() << "fooo";
	}
};

QTEST_MAIN(Util_test)
#include "Util_test.moc"