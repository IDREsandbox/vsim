#include <QTest>
#include <QDebug>
#include <QSignalSpy>

#include "ERSerializer.h"
#include "resources/EResourceGroup.h"
#include "resources/EResource.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"
#include "Util.h"

namespace fb = VSim::FlatBuffers;
class ERSerializer_test : public QObject {
	Q_OBJECT
private:

private slots:
void writeReadTest() {
	osg::ref_ptr<EResourceGroup> resources = new EResourceGroup;
	osg::ref_ptr<ECategoryGroup> cats = resources->categories();

	ECategory *cat1 = new ECategory;
	cat1->setCategoryName("cat1");
	cat1->setColor(QColor(255, 150, 50));
	cats->addChild(cat1);

	ECategory *cat2 = new ECategory;
	cat2->setCategoryName("cat2");
	cat2->setColor(QColor(0, 0, 199));
	cats->addChild(cat2);

	EResource *res;
	res = new EResource;
	res->setResourceName("res1");
	res->setAuthor("auth1");
	res->setERType(EResource::ANNOTATION);
	res->setResourceDescription("desc1");
	res->setResourcePath("path1");
	res->setGlobal(true);
	res->setCopyright(EResource::Copyright::PERMISSION);
	res->setMinYear(1994);
	res->setMaxYear(2004);
	res->setReposition(true);
	res->setAutoLaunch(false);
	res->setLocalRange(15.0f);
	res->setCategory(cat1);
	resources->addChild(res);

	res = new EResource;
	res->setResourceName("res2");
	res->setERType(EResource::FILE);
	res->setCategory(cat2);
	resources->addChild(res);

	// save to buffer
	flatbuffers::FlatBufferBuilder builder;
	flatbuffers::Offset<fb::ERTable> o_ers =
		ERSerializer::createERTable(resources.get(), &builder);
	builder.Finish(o_ers);

	// read from buffer
	//ERSerializer::readERTable();
	const fb::ERTable *table = flatbuffers::GetRoot<fb::ERTable>(builder.GetBufferPointer());

	osg::ref_ptr<EResourceGroup> nresources = new EResourceGroup;
	ERSerializer::readERTable(table, nresources);

	// check
	// categories
	ECategoryGroup *ncats = nresources->categories();
	QCOMPARE(ncats->getNumChildren(), 2);
	ECategory *ncat1 = ncats->category(0);
	QCOMPARE(ncat1->getCategoryName(), "cat1");
	QCOMPARE(ncat1->getColor(), QColor(255, 150, 50));
	ECategory *ncat2 = ncats->category(1);
	QCOMPARE(ncat2->getCategoryName(), "cat2");
	QCOMPARE(ncat2->getColor(), QColor(0, 0, 199));

	// resources
	QCOMPARE(nresources->getNumChildren(), 2);
	EResource *nres = nresources->getResource(0);
	QCOMPARE(nres->getERType(), EResource::ANNOTATION);
	QCOMPARE(nres->getResourceName(), "res1");
	QCOMPARE(nres->getAuthor(), "auth1");
	QCOMPARE(nres->getResourceDescription(), "desc1");
	QCOMPARE(nres->getResourcePath(), "path1");
	QCOMPARE(nres->getGlobal(), true);
	QCOMPARE(nres->getCopyright(), EResource::Copyright::PERMISSION);
	QCOMPARE(nres->getMinYear(), 1994);
	QCOMPARE(nres->getMaxYear(), 2004);
	QCOMPARE(nres->getReposition(), true);
	QCOMPARE(nres->getAutoLaunch(), false);
	QCOMPARE(nres->getLocalRange(), 15.0f);
	QCOMPARE(nres->getCategory(), ncat1);
	nres = nresources->getResource(1);
	QCOMPARE(nres->getResourceName(), "res2");
	QCOMPARE(nres->getERType(), EResource::FILE);
}
};

QTEST_MAIN(ERSerializer_test)
#include "ERSerializer_test.moc"