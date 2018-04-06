#include "FileUtil.h"
#include "VSimSerializer.h"
#include "NarrativeSerializer.h"
#include "ERSerializer.h"
#include <fstream>
#include <sstream>
#include <QDebug>
#include <unordered_set>

#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "resources/EResourceGroup.h"
#include "resources/EResource.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"
#include "resources/ERControl.h"
#include "deprecated/narrative/NarrativeOld.h"
#include "deprecated/resources/EResourcesList.h"
#include "deprecated/resources/EResourcesCategory.h"

#include "Group.h"

namespace fb = VSim::FlatBuffers;
bool FileUtil::readVSimFile(const std::string & path, VSimRoot * root)
{
	std::ifstream in(path, std::ios::binary);
	if (!in.good()) {
		return false;
	}
	return VSimSerializer::readStreamRobust(in, root);
}

bool FileUtil::writeVSimFile(const std::string & path, VSimRoot * root)
{
	std::ofstream out(path, std::ios::binary);
	if (!out.good()) {
		return false;
	}
	return VSimSerializer::writeStream(out, root);
}

bool FileUtil::importModel(const std::string & path, VSimRoot * root)
{
	return false;
}

bool FileUtil::importNarrativesStream(std::istream &in, NarrativeGroup *group)
{
	// read into buffer
	std::stringstream ss;
	ss << in.rdbuf();
	std::string s = ss.str();
	const uint8_t *buf = reinterpret_cast<const uint8_t*>(s.c_str());

	bool fb_match = fb::VerifyNarrativeTableBuffer(flatbuffers::Verifier(buf, s.length()));
	if (fb_match) {
		// reading flatbuffers
		const fb::NarrativeTable *fb_root = fb::GetNarrativeTable(s.c_str());
		NarrativeSerializer::readNarrativeTable(fb_root, group);
		return true;
	}

	// trying to read old stuff
	osg::ref_ptr<osg::Node> node = VSimSerializer::readOSGB(ss, false, false);
	if (!node) {
		qWarning() << "failed to load narrative file, not flatbuffer or osg";
		return false;
	}
	// try group, try narrative
	NarrativeOld *old = dynamic_cast<NarrativeOld*>(node.get());
	if (old) {
		group->addChild(new Narrative(old));
		return true;
	}
	return false;
}

bool FileUtil::exportNarrativesStream(std::ostream &out, const NarrativeGroup *group,
	const std::set<int> &selection)
{
	if (!out.good()) {
		return false;
	}

	// make a new group w/ copy of narratives
	NarrativeGroup g;
	for (auto i : selection) {
		Narrative *nar = dynamic_cast<Narrative*>(group->child(i));
		if (nar) {
			g.addChild(nar);
		}
	}

	flatbuffers::FlatBufferBuilder builder;
	auto o_table = NarrativeSerializer::createNarrativeTable(&builder, &g);
	fb::FinishNarrativeTableBuffer(builder, o_table);

	const char *buf = reinterpret_cast<const char*>(builder.GetBufferPointer());
	out.write(buf, builder.GetSize());

	return out.good();
}

bool FileUtil::importEResources(std::istream &in, EResourceGroup *group)
{
	// read into buffer
	std::stringstream ss;
	ss << in.rdbuf();
	std::string s = ss.str();
	const uint8_t *buf = reinterpret_cast<const uint8_t*>(s.c_str());

	bool fb_match = fb::VerifyERTableBuffer(flatbuffers::Verifier(buf, s.length()));
	if (fb_match) {
		// reading flatbuffers
		const fb::ERTable *fb_root = fb::GetERTable(s.c_str());
		ERSerializer::readERTable(fb_root, group);
		return true;
	}

	// trying to read old stuff
	osg::ref_ptr<osg::Node> node = VSimSerializer::readOSGB(ss, false, false);
	if (!node) {
		qWarning() << "failed to load resource file";
		return false;
	}
	// try group, try narrative
	EResourcesList *old = dynamic_cast<EResourcesList*>(node.get());
	if (old) {
		qInfo() << "loaded old eresourceslist";
		group->loadOld(old);
		return true;
	}

	// failure
	return false;
}

bool FileUtil::exportEResources(std::ostream &out, const EResourceGroup * group,
	const std::set<int>& selection)
{
	if (!out.good()) {
		return false;
	}

	// make a new group w/ copy of narratives
	EResourceGroup g;
	std::unordered_set<ECategory*> cats;
	for (auto i : selection) {
		EResource *res = group->getResource(i);
		if (res) {
			g.addChild(res);
		}
		ECategory *cat = res->category();
		if (cat && (cats.find(cat) == cats.end())) {
			cats.insert(cat);
			g.categories()->addChild(cat);
			cats.insert(res->category());
		}
	}

	flatbuffers::FlatBufferBuilder builder;
	auto o_table = ERSerializer::createERTable(&builder, &g);
	fb::FinishERTableBuffer(builder, o_table);

	const char *buf = reinterpret_cast<const char*>(builder.GetBufferPointer());
	out.write(buf, builder.GetSize());

	return out.good();
}
