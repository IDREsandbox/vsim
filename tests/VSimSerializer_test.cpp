#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include "VSimSerializer.h"
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include "Util.h"
#include "VSimRoot.h"
#include "narrative/Narrative2.h"
#include "narrative/NarrativeGroup.h"
#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include "ModelGroup.h"

namespace fb = VSim::FlatBuffers;
class VSimSerializer_test : public QObject {
	Q_OBJECT
private:

private slots:

void robustStreamTest() {
	VSimRoot *root = new VSimRoot;
	std::ifstream in("assets/test/cow_old.vsim", std::ios::binary);
	QVERIFY(in.good());
	bool ok = VSimSerializer::readStreamRobust(in, root);
	QVERIFY(ok);

	// debug root
	// root->debug();

	QCOMPARE(root->narratives()->getNumChildren(), 2);
	QCOMPARE(dynamic_cast<Narrative2*>(root->narratives()->child(0))->getTitle(), "nar2");
	QCOMPARE(dynamic_cast<Narrative2*>(root->narratives()->child(0))->getTitle(), "nar1");
	QCOMPARE(root->resources()->getNumChildren(), 1);
	QCOMPARE(root->resources()->getResource(0)->getResourceName(), "er1");
	//QCOMPARE(nroot->models()->child(0)->getName(), "cow.osg");
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
	VSimSerializer::writeOSGB(ss1, cow, false, false);
	std::string data1 = ss1.str();
	qInfo() << "write binary size:" << data1.size() << Util::toc() << "ms";

	Util::tic();
	osg::ref_ptr<osg::Node> cow1;
	cow1 = VSimSerializer::readOSGB(ss1, false, false);
	QVERIFY(cow1 != nullptr);
	QVERIFY(cow1->getName() == "cow.osg");
	qInfo() << "read binary" << Util::toc() << "ms";

	// write/read cow ascii
	Util::tic();
	std::stringstream ss2;
	VSimSerializer::writeOSGB(ss2, cow, true, false);
	std::string data2 = ss2.str();
	qInfo() << "write ascii size:" << data2.size() << Util::toc() << "ms";

	Util::tic();
	osg::ref_ptr<osg::Node> cow2;
	cow2 = VSimSerializer::readOSGB(ss2, true, true);
	QVERIFY(cow2 != nullptr);
	QVERIFY(cow2->getName() == "cow.osg");
	qInfo() << "read ascii" << Util::toc() << "ms";


	// write/read cow binary compressed
	Util::tic();
	std::stringstream ss3;
	VSimSerializer::writeOSGB(ss3, cow, false, true);
	std::string data3 = ss3.str();
	qInfo() << "write zlib size:" << data3.size() << Util::toc() << "ms";

	Util::tic();
	osg::ref_ptr<osg::Node> cow3;
	cow3 = VSimSerializer::readOSGB(ss3, false, true);
	QVERIFY(cow3 != nullptr);
	QVERIFY(cow3->getName() == "cow.osg");
	qInfo() << "read zlib size:" << Util::toc() << "ms";

	// test is reader is self-aware
	// write compressed, read ascii?
	// it doesn't work -> expect error and nullptr
	{
		std::stringstream ss;
		VSimSerializer::writeOSGB(ss, cow, false, true);
		std::string data = ss.str();
		qInfo() << "write zlib size:" << data.size();

		osg::ref_ptr<osg::Node> cow1;
		cow3 = VSimSerializer::readOSGB(ss, true, false);
		QVERIFY(cow1 == nullptr);
	}


	// write/read multiple binary
	{
		std::stringstream ss4;
		VSimSerializer::writeOSGB(ss4, cow, false, false);
		VSimSerializer::writeOSGB(ss4, cow, false, false);
		std::string data4 = ss4.str();

		// read multiple ascii
		osg::ref_ptr<osg::Node> cow4a;
		osg::ref_ptr<osg::Node> cow4b;
		cow4a = VSimSerializer::readOSGB(ss4, false, false);
		QVERIFY(cow4a != nullptr);
		QVERIFY(cow4a->getName() == "cow.osg");
		cow4b = VSimSerializer::readOSGB(ss4, false, false);
		QVERIFY(cow4b != nullptr);
		QVERIFY(cow4b->getName() == "cow.osg");
	}

	// write/read multiple ascii
	{
		std::stringstream ss4;
		VSimSerializer::writeOSGB(ss4, cow, true, false);
		VSimSerializer::writeOSGB(ss4, cow, true, false);
		std::string data4 = ss4.str();

		// read multiple ascii
		osg::ref_ptr<osg::Node> cow4a;
		osg::ref_ptr<osg::Node> cow4b;
		osg::ref_ptr<osg::Node> cow4c;
		cow4a = VSimSerializer::readOSGB(ss4, true, false);
		QVERIFY(cow4a != nullptr);
		QVERIFY(cow4a->getName() == "cow.osg");
		cow4b = VSimSerializer::readOSGB(ss4, true, false);
		QVERIFY(cow4b != nullptr);
		QVERIFY(cow4b->getName() == "cow.osg");
	}


	// write/read multiple compressed
	// doesn't work, second one is null
	{
		std::stringstream ss4;
		VSimSerializer::writeOSGB(ss4, cow, false, true);
		VSimSerializer::writeOSGB(ss4, cow, false, true);
		std::string data4 = ss4.str();
		qInfo() << "write zlibx2 size:" << data4.size();

		// read multiple ascii
		osg::ref_ptr<osg::Node> cow4a;
		osg::ref_ptr<osg::Node> cow4b;
		cow4a = VSimSerializer::readOSGB(ss4, false, true);
		QVERIFY(cow4a != nullptr);
		QVERIFY(cow4a->getName() == "cow.osg");
		cow4b = VSimSerializer::readOSGB(ss4, false, true);
		QVERIFY(cow4b == nullptr);
	}
}

void writeReadFlatbuffer() {
	osg::ref_ptr<VSimRoot> root = new VSimRoot;
	std::stringstream model_data;

	// add some narratives
	Narrative2 *nar = new Narrative2();
	nar->setTitle("nar1");
	root->narratives()->addChild(nar);
	root->narratives()->addChild(new Narrative2);
	// add some ers
	EResource *er = new EResource;
	er->setResourceName("er1");
	root->resources()->addChild(er);
	root->resources()->addChild(new EResource);

	// write to flatbuffer
	flatbuffers::FlatBufferBuilder builder;
	auto o_root = VSimSerializer::createRoot(&builder, root, model_data);
	fb::FinishRootBuffer(builder, o_root);
	// read from flatbuffer
	auto fb_root = fb::GetRoot(builder.GetBufferPointer());
	// verify buffer
	bool ok = fb::VerifyRootBuffer(flatbuffers::Verifier(builder.GetBufferPointer(), builder.GetSize()));
	QVERIFY(ok);

	osg::ref_ptr<VSimRoot> nroot = new VSimRoot;
	VSimSerializer::readRoot(fb_root, nroot);

	// check
	QCOMPARE(nroot->narratives()->getNumChildren(), 2);
	QCOMPARE(dynamic_cast<Narrative2*>(nroot->narratives()->child(0))->getTitle(), "nar1");
	QCOMPARE(nroot->resources()->getNumChildren(), 2);
	QCOMPARE(nroot->resources()->getResource(0)->getResourceName(), "er1");
}
void writeReadStream() {
	osg::ref_ptr<VSimRoot> root = new VSimRoot;

	// add some narratives
	Narrative2 *nar = new Narrative2();
	nar->setTitle("nar1");
	root->narratives()->addChild(nar);
	root->narratives()->addChild(new Narrative2);
	// add some ers
	EResource *er = new EResource;
	er->setResourceName("er1");
	root->resources()->addChild(er);
	root->resources()->addChild(new EResource);
	// add some models
	osg::ref_ptr<osg::Node> cow;
	cow = osgDB::readNodeFile("assets/test/cow.osgt");
	root->models()->addChild(cow);

	// write to stream
	std::stringstream stream;
	VSimSerializer::writeStream(stream, root);

	// read from stream
	osg::ref_ptr<VSimRoot> nroot = new VSimRoot;
	bool ok = VSimSerializer::readStream(stream, nroot);

	// check models
	QCOMPARE(ok, true);
	QCOMPARE(nroot->narratives()->getNumChildren(), 2);
	QCOMPARE(dynamic_cast<Narrative2*>(nroot->narratives()->child(0))->getTitle(), "nar1");
	QCOMPARE(nroot->resources()->getNumChildren(), 2);
	QCOMPARE(nroot->resources()->getResource(0)->getResourceName(), "er1");
	QCOMPARE(nroot->models()->child(0)->getName(), "cow.osg");
}

void writeReadTestFull() {
}

};

QTEST_MAIN(VSimSerializer_test)
#include "VSimSerializer_test.moc"