#include <QTest>
#include <QDebug>
#include <osg/Group>

#include "narrative/NarrativeModel.h"

#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeSlideLabels.h"


class NarrativeModel_test : public QObject {
	Q_OBJECT

private:

	osg::Group *initOSG();

private slots:
	void emptyTest();

	void narrativeSetData();
	void narrativeGetData();
	void narrativeInsert();
	void narrativeRemove();

	void removeErrors();

};

osg::Group *NarrativeModel_test::initOSG() {
	osg::Group *root = new osg::Group;

	Narrative2 *nar;
	NarrativeSlide *slide;
	NarrativeSlideLabels *label;

	nar = new Narrative2;
	nar->setTitle("title one");
	nar->setDescription("description one");
	nar->setAuthor("the first author");
	root->addChild(nar);

	for (int i = 0; i < 4; i++) {
		slide = new NarrativeSlide;
		slide->setCameraMatrix(osg::Matrix::lookAt(osg::Vec3(1, 1, 1), osg::Vec3(0, 0, 1), osg::Vec3()));
		slide->setDuration(1.0 + i);
		slide->setStayOnNode(true);
		slide->setTransitionDuration(1.1);
		nar->addChild(nar);

		for (int j = 0; j < 3; j++) {
			label = new NarrativeSlideLabels;
			label->setrX(.3);
			label->setrY(.3 + i*.1);
			label->setrW(.2 + i*.01);
			label->setrH(.1 + i*.001);
			label->setText("foo - " + std::to_string(i * 10 + j));
			slide->addChild(label);
		}
	}
	
	nar = new Narrative2;
	nar->setTitle("title two");
	nar->setDescription("description two");
	nar->setAuthor("the second author");
	root->addChild(nar);

	return root;
	//qInfo() << "running basic test";
}


void NarrativeModel_test::emptyTest() {
	qInfo() << "running basic test";
	osg::ref_ptr<osg::Group> root = new osg::Group;
	NarrativeModel model(nullptr, root, nullptr);

	int rows = model.rowCount();
	int columns = model.columnCount();

	QVERIFY(rows == 0);
	QVERIFY(columns == 3);
}

void NarrativeModel_test::narrativeSetData()
{
	osg::ref_ptr<osg::Group> g = new osg::Group;
	NarrativeModel model(nullptr, g, nullptr);
	QVERIFY(model.insertRows(0, 2));
	model.setData(model.index(0, 0), "my title");
	model.setData(model.index(0, 1), "my description");
	model.setData(model.index(0, 2), "my author");
	model.setData(model.index(1, 0), "my title2");
	model.setData(model.index(1, 1), "my description2");
	model.setData(model.index(1, 2), "my author2");

	Narrative2 *nar = dynamic_cast<Narrative2*>(g->getChild(0));
	QVERIFY(nar->getTitle() == "my title");
	QVERIFY(nar->getDescription() == "my description");
	QVERIFY(nar->getAuthor() == "my author");

	nar = dynamic_cast<Narrative2*>(g->getChild(1));
	QVERIFY(nar->getTitle() == "my title2");
	QVERIFY(nar->getDescription() == "my description2");
	QVERIFY(nar->getAuthor() == "my author2");
}

void NarrativeModel_test::narrativeGetData()
{
	osg::ref_ptr<osg::Group> g = initOSG();
	NarrativeModel model(nullptr, g, nullptr);

	QVERIFY(model.data(model.index(0, 0), Qt::DisplayRole).toString() == "title one");
	QVERIFY(model.data(model.index(0, 1), Qt::DisplayRole).toString() == "description one");
	QVERIFY(model.data(model.index(0, 2), Qt::DisplayRole).toString() == "the first author");
}

void NarrativeModel_test::narrativeInsert()
{
	NarrativeModel model(nullptr, new osg::Group, nullptr);
	QVERIFY(model.insertRows(0, 5));
	// {0, 1, 2, 3, 4}
	for (int i = 0; i < 5; i++) {
		model.setData(model.index(i, 0), QString("Title ") + QString::number(i));
	}
	for (int i = 0; i < 5; i++) {
		QString str = model.data(model.index(i, 0), Qt::DisplayRole).toString();
		QVERIFY(str == (QString("Title ") + QString::number(i)));
	}
}

void NarrativeModel_test::narrativeRemove()
{
	// initializing
	osg::Group *g = new osg::Group;
	for (int i = 0; i < 5; i++) {
		// {0, 1, 2, 3, 4}
		Narrative2 *nar = new Narrative2;
		nar->setTitle("Title " + std::to_string(i));
		g->addChild(nar);
	}
	NarrativeModel model(nullptr, g, nullptr);
	QString str;
	// remove 2 rows from beginning
	// {2, 3, 4}
	QVERIFY(model.removeRows(0, 2));
	str = model.data(model.index(0, 0), Qt::DisplayRole).toString();
	QVERIFY(str == "Title 2");
	// remove 1 rows in middle
	// {2, 4}
	QVERIFY(model.removeRows(1, 1));
	str = model.data(model.index(1, 0), Qt::DisplayRole).toString();
	QVERIFY(str == "Title 4");
	QVERIFY(model.rowCount() == 2);
}
void NarrativeModel_test::removeErrors()
{
	NarrativeModel model(nullptr, new osg::Group, nullptr);
	model.insertRows(0, 2);

	// error cases
	QVERIFY(model.removeRows(10, 0) == false);
	QVERIFY(model.removeRows(-1, 2) == false);
	QVERIFY(model.removeRows(0, 10) == false);
	QVERIFY(model.removeRows(0, -1) == false);
}

QTEST_MAIN(NarrativeModel_test)
#include "NarrativeModel_test.moc"
