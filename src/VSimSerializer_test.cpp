#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "VSimSerializer.h"
#include "Core/Util.h"
#include "Core/LockTable.h"
#include "VSimRoot.h"
#include "narrative/Narrative.h"
#include "narrative/NarrativeGroup.h"
#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"
#include "Model/ModelGroup.h"
#include "Model/Model.h"
#include "Model/ModelSerializer.h"

namespace fb = VSim::FlatBuffers;
class VSimSerializer_test : public QObject {
	Q_OBJECT
private:

private slots:

#ifdef BROKENOLDTESTS
void robustStreamTestOld() {
	VSimRoot root;
	std::ifstream in("assets/test/cow_old.vsim", std::ios::binary);
	QVERIFY(in.good());
	bool ok = VSimSerializer::readStreamRobust(in, &root, {});
	QVERIFY(ok);

	QCOMPARE(root.narratives()->size(), 2u);
	// narrative titles
	QCOMPARE(root.narratives()->child(0)->getTitle().c_str(), "nar1");
	QCOMPARE(root.narratives()->child(1)->getTitle().c_str(), "nar2");
	QCOMPARE(root.resources()->size(), 1u);
	QCOMPARE(root.resources()->getResource(0)->getResourceName().c_str(), "er1");

	// check model information
	//auto *settings = root.settings();
	//QVERIFY(settings->model_information);
	//auto *info = settings->model_information.get();
	//auto *info = &root.modelInformation();
	//QCOMPARE(info->authors.c_str(), "authors");
	//QCOMPARE(info->contributors.c_str(), "contributors");
	//QCOMPARE(info->model_name.c_str(), "name");
	//QCOMPARE(info->place_of_publication.c_str(), "place");
	//QCOMPARE(info->url.c_str(), "url");
	//QCOMPARE(info->project_date_julian_day, (size_t)QDate(2018, 4, 5).toJulianDay());
}
void robustStreamTest() {
	// read new model
	VSimRoot root;
	std::ifstream in("assets/test/cow_new.vsim", std::ios::binary);
	QVERIFY(in.good());
	bool ok = VSimSerializer::readStreamRobust(in, &root, {});
	QVERIFY(ok);

	QCOMPARE(root.narratives()->size(), 2u);
	// narrative titles
	QCOMPARE(root.narratives()->child(0)->getTitle().c_str(), "nar1");
	QCOMPARE(root.narratives()->child(1)->getTitle().c_str(), "nar2");
	QCOMPARE(root.resources()->size(), 1u);
	QCOMPARE(root.resources()->getResource(0)->getResourceName().c_str(), "er1");

	// check model information
	auto *info = &root.modelInformation();
	QCOMPARE(info->authors.c_str(), "authors");
	QCOMPARE(info->contributors.c_str(), "contributors");
	QCOMPARE(info->model_name.c_str(), "name");
	QCOMPARE(info->place_of_publication.c_str(), "place");
	QCOMPARE(info->url.c_str(), "url");
	QCOMPARE(info->project_date_julian_day, (size_t)QDate(2018, 4, 5).toJulianDay());
}

void oldERTest() {
	VSimRoot root;
	std::ifstream in("assets/test/local_global_old.vsim", std::ios::binary);
	QVERIFY(in.good());

	bool ok = VSimSerializer::readStreamRobust(in, &root, {});
	QVERIFY(ok);

	QCOMPARE(root.resources()->size(), 2u);
	auto res1 = root.resources()->getResource(0);
	auto res2 = root.resources()->getResource(1);
	QCOMPARE(res1->getResourceName().c_str(), "local");
	QCOMPARE(res1->category()->getCategoryName().c_str(), "l");
	QCOMPARE(res1->getGlobal(), false);
	QCOMPARE(res2->getResourceName().c_str(), "global");
	QCOMPARE(res2->category()->getCategoryName().c_str(), "g");
	QCOMPARE(res2->getGlobal(), true);
}

void importExportER() {
	// export
	// er1 cat1
	// er2 duplicate(2)
	// er5 cat1

	// existing
	// er4 duplicate(3)

	// expected
	// er4 duplicate(3)
	// er1 cat1
	// er2 duplicate(3)
	// er5 cat1

	// export group
	EResourceGroup group1;
	auto cat1 = std::make_shared<ECategory>();
	auto cat2 = std::make_shared<ECategory>();
	cat1->setCategoryName("cat1");
	cat2->setCategoryName("cat2");
	group1.categories()->append(cat1);
	group1.categories()->append(cat2);

	auto er1 = std::make_shared<EResource>();
	auto er2 = std::make_shared<EResource>();
	auto er3 = std::make_shared<EResource>();
	er1->setResourceName("er1");
	er2->setResourceName("er2");
	er3->setResourceName("er3");
	group1.append(er1);
	group1.append(er2);
	group1.append(er3);
	er1->setCategory(cat1);
	er2->setCategory(cat2);

	std::stringstream ss;
	qDebug() << "export ers stream";
	VSimSerializer::exportEResources(ss, &group1, { 0, 1 }, {});

	// existing group
	EResourceGroup group2;

	qDebug() << "import ers stream";
	VSimSerializer::importEResources(ss, &group2, {});

	// merged group
	QCOMPARE(group2.size(), 2u);
	QCOMPARE(group2.getResource(0)->getResourceName().c_str(), "er1");
	QCOMPARE(group2.getResource(1)->getResourceName().c_str(), "er2");
	QCOMPARE(group2.getResource(0)->category()->getCategoryName().c_str(), "cat1");
	QCOMPARE(group2.getResource(1)->category()->getCategoryName().c_str(), "cat2");
	QCOMPARE(group2.categories()->size(), 2u);
	QCOMPARE(group2.categories()->category(0)->getCategoryName().c_str(), "cat1");
	QCOMPARE(group2.categories()->category(1)->getCategoryName().c_str(), "cat2");	
}

void oldImportNarratives() {
	NarrativeGroup group;
	qDebug() << "reading old narrative file";
	std::ifstream ifs("assets/test/old_nar.nar", std::ios::binary);
	bool ok = VSimSerializer::importNarrativesStream(ifs, &group);
	QVERIFY(ok);

	QCOMPARE(group.size(), 1u);
	QCOMPARE(group.narrative(0)->getTitle().c_str(), "nar1");
}

void importExportNarratives() {
	NarrativeGroup group1;
	auto nar1 = std::make_shared<Narrative>();
	auto nar2 = std::make_shared<Narrative>();
	auto nar3 = std::make_shared<Narrative>();

	nar1->setTitle("nar1");
	nar2->setTitle("nar2");
	nar3->setTitle("nar3");

	group1.append(nar1);
	group1.append(nar2);
	group1.append(nar3);

	std::stringstream ss;
	qDebug() << "export narrative stream";
	VSimSerializer::exportNarrativesStream(ss, &group1, { 0, 2 });

	NarrativeGroup group2;
	auto nar4 = std::make_shared<Narrative>();
	nar4->setTitle("nar4");
	group2.append(nar4);

	qDebug() << "import narrative stream";
	VSimSerializer::importNarrativesStream(ss, &group2);
	
	QCOMPARE(group2.size(), 3u);
	QCOMPARE(group2.narrative(0)->getTitle().c_str(), "nar4");
	QCOMPARE(group2.narrative(1)->getTitle().c_str(), "nar1");
	QCOMPARE(group2.narrative(2)->getTitle().c_str(), "nar3");
}

void oldImportResources() {
	EResourceGroup group;
	qDebug() << "reading old narrative file";
	std::ifstream ifs("assets/test/old_ers.ere", std::ios::binary);
	bool ok = VSimSerializer::importEResources(ifs, &group, {});
	QVERIFY(ok);

	QCOMPARE(group.size(), 2u);
	auto res1 = group.getResource(0);
	auto res2 = group.getResource(1);
	QCOMPARE(res1->getResourceName().c_str(), "local");
	QCOMPARE(res1->category()->getCategoryName().c_str(), "l");
	QCOMPARE(res1->getGlobal(), false);
	QCOMPARE(res2->getResourceName().c_str(), "global");
	QCOMPARE(res2->category()->getCategoryName().c_str(), "g");
	QCOMPARE(res2->getGlobal(), true);
	QCOMPARE(group.categories()->size(), 2u);
}

void osgStreamTest() {
	// read cow (osg)
	osg::ref_ptr<osg::Node> cow;
	cow = osgDB::readNodeFile("assets/test/cow.osgt");
	QVERIFY(cow != nullptr);
	QVERIFY(cow->getName() == "cow.osg");

	// write/read cow binary
	Util::tic();
	std::stringstream ss1;
	ModelSerializer::writeOSGB(ss1, cow, false, false);
	std::string data1 = ss1.str();
	qInfo() << "write binary size:" << data1.size() << Util::toc() << "ms";

	Util::tic();
	osg::ref_ptr<osg::Node> cow1;
	cow1 = ModelSerializer::readOSGB(ss1, false, false);
	QVERIFY(cow1 != nullptr);
	QVERIFY(cow1->getName() == "cow.osg");
	qInfo() << "read binary" << Util::toc() << "ms";

	// write/read cow ascii
	Util::tic();
	std::stringstream ss2;
	ModelSerializer::writeOSGB(ss2, cow, true, false);
	std::string data2 = ss2.str();
	qInfo() << "write ascii size:" << data2.size() << Util::toc() << "ms";

	Util::tic();
	osg::ref_ptr<osg::Node> cow2;
	cow2 = ModelSerializer::readOSGB(ss2, true, true);
	QVERIFY(cow2 != nullptr);
	QVERIFY(cow2->getName() == "cow.osg");
	qInfo() << "read ascii" << Util::toc() << "ms";


	// write/read cow binary compressed
	Util::tic();
	std::stringstream ss3;
	ModelSerializer::writeOSGB(ss3, cow, false, true);
	std::string data3 = ss3.str();
	qInfo() << "write zlib size:" << data3.size() << Util::toc() << "ms";

	Util::tic();
	osg::ref_ptr<osg::Node> cow3;
	cow3 = ModelSerializer::readOSGB(ss3, false, true);
	QVERIFY(cow3 != nullptr);
	QVERIFY(cow3->getName() == "cow.osg");
	qInfo() << "read zlib size:" << Util::toc() << "ms";

	// test is reader is self-aware
	// write compressed, read ascii?
	// it doesn't work -> expect error and nullptr
	{
		std::stringstream ss;
		ModelSerializer::writeOSGB(ss, cow, false, true);
		std::string data = ss.str();
		qInfo() << "write zlib size:" << data.size();

		osg::ref_ptr<osg::Node> cow1;
		cow3 = ModelSerializer::readOSGB(ss, true, false);
		QVERIFY(cow1 == nullptr);
	}


	// write/read multiple binary
	{
		std::stringstream ss4;
		ModelSerializer::writeOSGB(ss4, cow, false, false);
		ModelSerializer::writeOSGB(ss4, cow, false, false);
		std::string data4 = ss4.str();

		// read multiple ascii
		osg::ref_ptr<osg::Node> cow4a;
		osg::ref_ptr<osg::Node> cow4b;
		cow4a = ModelSerializer::readOSGB(ss4, false, false);
		QVERIFY(cow4a != nullptr);
		QVERIFY(cow4a->getName() == "cow.osg");
		cow4b = ModelSerializer::readOSGB(ss4, false, false);
		QVERIFY(cow4b != nullptr);
		QVERIFY(cow4b->getName() == "cow.osg");
	}

	// write/read multiple ascii
	{
		std::stringstream ss4;
		ModelSerializer::writeOSGB(ss4, cow, true, false);
		ModelSerializer::writeOSGB(ss4, cow, true, false);
		std::string data4 = ss4.str();

		// read multiple ascii
		osg::ref_ptr<osg::Node> cow4a;
		osg::ref_ptr<osg::Node> cow4b;
		osg::ref_ptr<osg::Node> cow4c;
		cow4a = ModelSerializer::readOSGB(ss4, true, false);
		QVERIFY(cow4a != nullptr);
		QVERIFY(cow4a->getName() == "cow.osg");
		cow4b = ModelSerializer::readOSGB(ss4, true, false);
		QVERIFY(cow4b != nullptr);
		QVERIFY(cow4b->getName() == "cow.osg");
	}


	// write/read multiple compressed
	// doesn't work, second one is null
	{
		std::stringstream ss4;
		ModelSerializer::writeOSGB(ss4, cow, false, true);
		ModelSerializer::writeOSGB(ss4, cow, false, true);
		std::string data4 = ss4.str();
		qInfo() << "write zlibx2 size:" << data4.size();

		// read multiple ascii
		osg::ref_ptr<osg::Node> cow4a;
		osg::ref_ptr<osg::Node> cow4b;
		cow4a = ModelSerializer::readOSGB(ss4, false, true);
		QVERIFY(cow4a != nullptr);
		QVERIFY(cow4a->getName() == "cow.osg");
		cow4b = ModelSerializer::readOSGB(ss4, false, true);
		QVERIFY(cow4b == nullptr);
	}
}

void writeReadFlatbuffer() {
	VSimRoot root;
	std::stringstream model_data;

	// add some narratives
	auto nar = std::make_shared<Narrative>();
	nar->setTitle("nar1");
	root.narratives()->append(nar);
	root.narratives()->append(std::make_shared<Narrative>());
	// add some ers
	auto er = std::make_shared<EResource>();
	er->setResourceName("er1");
	root.resources()->append(er);
	root.resources()->append(std::make_shared<EResource>());

	// write to flatbuffer
	flatbuffers::FlatBufferBuilder builder;
	auto o_root = VSimSerializer::createRoot(&builder, &root, model_data, {});
	fb::FinishRootBuffer(builder, o_root);
	// read from flatbuffer
	auto fb_root = fb::GetRoot(builder.GetBufferPointer());
	// verify buffer
	bool ok = fb::VerifyRootBuffer(flatbuffers::Verifier(builder.GetBufferPointer(), builder.GetSize()));
	QVERIFY(ok);

	VSimRoot nroot;
	VSimSerializer::readRoot(fb_root, &nroot, {});

	// check
	QCOMPARE(nroot.narratives()->size(), 2u);
	QCOMPARE(nroot.narratives()->child(0)->getTitle().c_str(), "nar1");
	QCOMPARE(nroot.resources()->size(), 2u);
	QCOMPARE(nroot.resources()->getResource(0)->getResourceName().c_str(), "er1");
}
void writeReadStream() {
	VSimRoot root;

	// add some narratives
	auto nar = std::make_shared<Narrative>();
	nar->setTitle("nar1");
	root.narratives()->append(nar);
	root.narratives()->append(std::make_shared<Narrative>());
	// add some ers
	auto er = std::make_shared<EResource>();
	er->setResourceName("er1");
	root.resources()->append(er);
	root.resources()->append(std::make_shared<EResource>());
	// add some models
	osg::ref_ptr<osg::Node> cow;
	cow = osgDB::readNodeFile("assets/test/cow.osgt");
	auto model = std::make_shared<Model>();
	model->setFile("assets/test/cow.osgt", cow);
	model->setName("cow");

	root.models()->addModel(model);

	// write to stream
	std::stringstream stream;
	VSimSerializer::writeStream(stream, &root, {});

	// read from stream
	VSimRoot nroot;
	bool ok = VSimSerializer::readStream(stream, &nroot, {});

	// check models
	QCOMPARE(ok, true);
	QCOMPARE(nroot.narratives()->size(), 2u);
	QCOMPARE(nroot.narratives()->child(0)->getTitle().c_str(), "nar1");
	QCOMPARE(nroot.resources()->size(), 2u);
	QCOMPARE(nroot.resources()->getResource(0)->getResourceName().c_str(), "er1");
	QCOMPARE(nroot.models()->group()->child(0)->node()->getName().c_str(), "cow.osg");
}

#endif

void writeReadSettings() {
	VSimRoot root;

	LockTable lt;
	lt.lockWithPassword("aaaa");
	root.setLockTable(lt);

	root.setSettingsLocked(true);
	root.setRestrictToCurrent(true);
	root.setNavigationLocked(true);

	// TODO: more complete test?

	root.modelInformation().authors = "authors";
	root.navigationSettings().base_speed = 10;
	root.graphicsSettings().camera_settings->fovy = 33;
	root.windowSettings().ebar_size = 111;
	root.setExpirationDate(QDate::currentDate());

	// write to stream
	std::stringstream stream;
	bool wok = VSimSerializer::writeStream(stream, &root, {});

	QCOMPARE(wok, true);

	std::istream &in = stream;

	// read from stream
	VSimRoot nroot;
	bool rok = VSimSerializer::readStream(stream, &nroot, {});

	QCOMPARE(rok, true);
	QCOMPARE(nroot.settingsLocked(), true);
	QCOMPARE(nroot.restrictedToCurrent(), true);
	QCOMPARE(nroot.navigationLocked(), true);

	QCOMPARE(nroot.modelInformation().authors.c_str(), "authors");
	QCOMPARE(nroot.navigationSettings().base_speed, 10.f);
	QCOMPARE(nroot.graphicsSettings().camera_settings->fovy, 33.f);
	QCOMPARE(nroot.windowSettings().ebar_size, 111);
	QCOMPARE(nroot.expirationDate(), QDate::currentDate());
}

};

QTEST_MAIN(VSimSerializer_test)
#include "VSimSerializer_test.moc"