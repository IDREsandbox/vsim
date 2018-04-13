#include <QTest>
#include <QDebug>
#include <QSignalSpy>

#include "resources/EResourceGroup.h"
#include "resources/EResource.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"

class EResource_test : public QObject {
	Q_OBJECT
private:

private slots:
void mergeTest() {
	// existing
	// er4 duplicate(3)

	// other
	// er1 cat1
	// er2 duplicate(2)
	// er3 cat1

	// expected
	// er4 duplicate(3)
	// er1 cat1
	// er2 duplicate(3)
	// er5 cat1

	QUndoStack stack;

	// existing group
	EResourceGroup group1;
	ECategory *cat3 = new ECategory;
	cat3->setCategoryName("duplicate");
	group1.categories()->addChild(cat3);

	EResource *er4 = new EResource;
	er4->setResourceName("er4");
	group1.addChild(er4);
	er4->setCategory(cat3);

	ECategory *cat1, *cat2;

	{ // braces check destruction
		// export group
		EResourceGroup group2;
		cat1 = new ECategory;
		cat2 = new ECategory;
		cat1->setCategoryName("cat1");
		cat2->setCategoryName("duplicate");
		group2.categories()->addChild(cat1);
		group2.categories()->addChild(cat2);

		EResource *er1 = new EResource;
		EResource *er2 = new EResource;
		EResource *er3 = new EResource;
		er1->setResourceName("er1");
		er2->setResourceName("er2");
		er3->setResourceName("er3");
		group2.addChild(er1);
		group2.addChild(er2);
		group2.addChild(er3);
		er1->setCategory(cat1);
		er2->setCategory(cat2);
		er3->setCategory(cat1);

		auto *cmd = new QUndoCommand();
		EResourceGroup::mergeCommand(&group1, &group2, cmd);
		stack.push(cmd);
	}

	// merged group
	QCOMPARE(group1.getNumChildren(), 4);
	QCOMPARE(group1.getResource(0)->getResourceName(), "er4");
	QCOMPARE(group1.getResource(1)->getResourceName(), "er1");
	QCOMPARE(group1.getResource(2)->getResourceName(), "er2");
	QCOMPARE(group1.getResource(3)->getResourceName(), "er3");
	QCOMPARE(group1.getResource(0)->getCategory(), cat3);
	QCOMPARE(group1.getResource(2)->getCategory(), cat3);
	QCOMPARE(group1.getResource(1)->getCategory(), cat1);
	QCOMPARE(group1.getResource(3)->getCategory(), cat1);
	QCOMPARE(group1.categories()->getNumChildren(), 2);
	QCOMPARE(group1.categories()->category(0)->getCategoryName(), "duplicate");
	QCOMPARE(group1.categories()->category(1)->getCategoryName(), "cat1");

	stack.undo();
	QCOMPARE(group1.getNumChildren(), 1);
	QCOMPARE(group1.getResource(0)->getResourceName(), "er4");
	QCOMPARE(group1.getResource(0)->getCategory(), cat3);
	QCOMPARE(group1.categories()->getNumChildren(), 1);
	QCOMPARE(group1.categories()->category(0)->getCategoryName(), "duplicate");

	stack.redo();
	stack.undo();
}
};

QTEST_MAIN(EResource_test)
#include "EResource_test.moc"