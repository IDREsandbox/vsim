#include <QTest>
#include <QDebug>
#include "Util.h"
#include <string>
#include <vector>
#include <iostream>
#include <cstddef>

struct NameYearTuple {
	std::string name;
	int begin;
	int end;
};

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
			Util::nodeTimeInName(tuple.name, &begin, &end);
			QCOMPARE(begin, tuple.begin);
			QCOMPARE(end, tuple.end);
		}
	}

	void isDescendantTest() {
		QWidget w1;
		QWidget *w2 = new QWidget(&w1);
		QWidget *w3 = new QWidget(w2);
		QWidget w4;
		QCOMPARE(Util::isDescendant(&w1, &w1), true);
		QCOMPARE(Util::isDescendant(&w1, w2), true);
		QCOMPARE(Util::isDescendant(&w1, w3), true);
		QCOMPARE(Util::isDescendant(w2, w3), true);
		QCOMPARE(Util::isDescendant(&w1, &w4), false);
		QCOMPARE(Util::isDescendant(w2, &w1), false);
		QCOMPARE(Util::isDescendant(w3, &w4), false);
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

	void angleWrap() {
		// 0 -> 0
		QVERIFY(Util::doubleEq(Util::angleWrap(0.0), 0.0));

		// -1 -> 2PI - 1
		QVERIFY(Util::doubleEq(Util::angleWrap(2*M_PI - 1.0), 2*M_PI - 1.0));

		// 360 -> 0
		QVERIFY(Util::doubleEq(Util::angleWrap(Util::rad(360)), 0.0));

		// 2PI + 1 -> 1
		QVERIFY(Util::doubleEq(Util::angleWrap(1.0 + 2 * M_PI), 1.0));
	}

	void closestAngle() {
		// 10, 350 -> -10
		QVERIFY(Util::doubleEq(
			Util::closestAngle(Util::rad(10), Util::rad(350)), Util::rad(-10)));

		// 0, 360 -> 0
		QVERIFY(Util::doubleEq(
			Util::closestAngle(0.0, 2*M_PI), 0.0));
	}

	void matToYPR() {
		// basic north facing camera matrix
		osg::Matrix base = Util::baseCamera();
		double y, p, r;

		// north facing
		Util::matToYPR(base, &y, &p, &r);
		QVERIFY(Util::doubleEq(y, 0));
		QVERIFY(Util::doubleEq(p, 0));
		QVERIFY(Util::doubleEq(r, 0));

		// west facing
		auto east = base * osg::Matrix::rotate(M_PI_2, osg::Vec3(0, 0, 1));
		Util::matToYPR(east, &y, &p, &r);
		QVERIFY(Util::doubleEq(y, M_PI_2));
		QVERIFY(Util::doubleEq(p, 0));
		QVERIFY(Util::doubleEq(r, 0));

		// up facing, special case for yaw
		auto up = base * osg::Matrix::rotate(M_PI_2, osg::Vec3(1, 0, 0));
		Util::matToYPR(up, &y, &p, &r);
		QVERIFY(Util::doubleEq(y, 0));
		QVERIFY(Util::doubleEq(p, M_PI_2));
		QVERIFY(Util::doubleEq(r, 0));

		// banked 45 right
		auto right = base * osg::Matrix::rotate(M_PI_4, osg::Vec3(0, 1, 0));
		Util::matToYPR(right, &y, &p, &r);
		QVERIFY(Util::doubleEq(y, 0));
		QVERIFY(Util::doubleEq(p, 0));
		QVERIFY(Util::doubleEq(r, M_PI_4));
	}

	void yprToMat() {
		double y, p, r;
		y = 1.5;
		p = .9;
		r = .3;
		double y2, p2, r2;
		Util::matToYPR(Util::yprToMat(y, p, r), &y2, &p2, &r2);
		QVERIFY(Util::doubleEq(y, y2));
		QVERIFY(Util::doubleEq(p, p2));
		QVERIFY(Util::doubleEq(r, r2));
	}

};

QTEST_MAIN(Util_test)
#include "Util_test.moc"