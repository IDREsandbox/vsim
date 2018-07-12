#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include <osg/Matrix>

#include "ERSerializer.h"
#include "resources/EResourceGroup.h"
#include "resources/EResource.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"
#include "Core/Util.h"
#include <QMatrix4x4>

namespace fb = VSim::FlatBuffers;
class ERSerializer_test : public QObject {
	Q_OBJECT
private:

private slots:
void writeReadTest() {
	EResourceGroup resources;
	ECategoryGroup *cats = resources.categories();

	auto cat1 = std::make_shared<ECategory>();
	cat1->setCategoryName("cat1");
	cat1->setColor(QColor(255, 150, 50));
	cats->append(cat1);

	auto cat2 = std::make_shared<ECategory>();
	cat2->setCategoryName("cat2");
	cat2->setColor(QColor(0, 0, 199));
	cats->append(cat2);

	auto res = std::make_shared<EResource>();
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
	res->setAutoLaunch(EResource::OFF);
	res->setLocalRange(15.0f);
	res->setCategory(cat1);

	osg::Matrix view_matrix;
	osg::Vec3 eye(12.0f, 12.0f, 12.0f);
	osg::Vec3 target(11.9f, 3.14f, -8.0f);
	osg::Vec3 up(0.0f, 0.0f, 1.0f);
	view_matrix = osg::Matrix::lookAt(eye, target, up);
	osg::Matrix camera_matrix = osg::Matrix::inverse(view_matrix);
	res->setCameraMatrix(camera_matrix);

	resources.append(res);

	res = std::make_shared<EResource>();
	res->setResourceName("res2");
	res->setERType(EResource::FILE);
	res->setCategory(cat2);
	resources.append(res);

	// save to buffer
	flatbuffers::FlatBufferBuilder builder;
	flatbuffers::Offset<fb::ERTable> o_ers =
		ERSerializer::createERTable(&builder, &resources);
	builder.Finish(o_ers);

	// read from buffer
	//ERSerializer::readERTable();
	const fb::ERTable *table = flatbuffers::GetRoot<fb::ERTable>(builder.GetBufferPointer());

	EResourceGroup nresources;
	ERSerializer::readERTable(table, &nresources);

	// check
	// categories
	ECategoryGroup *ncats = resources.categories();
	QCOMPARE(ncats->size(), 2u);
	ECategory *ncat1 = ncats->category(0);
	QCOMPARE(ncat1->getCategoryName().c_str(), "cat1");
	QCOMPARE(ncat1->getColor(), QColor(255, 150, 50));
	ECategory *ncat2 = ncats->category(1);
	QCOMPARE(ncat2->getCategoryName().c_str(), "cat2");
	QCOMPARE(ncat2->getColor(), QColor(0, 0, 199));

	// resources
	QCOMPARE(resources.size(), 2u);
	EResource *nres = resources.getResource(0);
	QCOMPARE(nres->getERType(), EResource::ANNOTATION);
	QCOMPARE(nres->getResourceName(), std::string("res1"));
	QCOMPARE(nres->getAuthor(), std::string("auth1"));
	QCOMPARE(nres->getResourceDescription(), std::string("desc1"));
	QCOMPARE(nres->getResourcePath(), std::string("path1"));
	QCOMPARE(nres->getGlobal(), true);
	QCOMPARE(nres->getCopyright(), EResource::Copyright::PERMISSION);
	QCOMPARE(nres->getMinYear(), 1994);
	QCOMPARE(nres->getMaxYear(), 2004);
	QCOMPARE(nres->getReposition(), true);
	QCOMPARE(nres->getAutoLaunch(), EResource::OFF);
	QCOMPARE(nres->getLocalRange(), 15.0f);
	QCOMPARE(nres->category(), ncat1);

	auto cm = nres->getCameraMatrix();
	QVERIFY(Util::osgMatrixEq(nres->getCameraMatrix(), camera_matrix, .00001));

	nres = resources.getResource(1);
	QCOMPARE(nres->getResourceName(), std::string("res2"));
	QCOMPARE(nres->getERType(), EResource::FILE);
}
};

QTEST_MAIN(ERSerializer_test)
#include "ERSerializer_test.moc"