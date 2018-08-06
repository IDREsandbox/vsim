#include "VSimSerializer.h"

#include <flatbuffers/flatbuffers.h>
#include <sstream>
#include <iterator>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <unordered_set>

#include "Canvas/CanvasSerializer.h"
#include "NarrativeSerializer.h"
#include "ERSerializer.h"
#include "VSimRoot.h"
#include "Model/ModelGroup.h"
#include "Model/Model.h"
#include "Model/ModelSerializer.h"
#include "Core/Util.h"

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

namespace fb = VSim::FlatBuffers;

bool VSimSerializer::readStreamRobust(std::istream & in, VSimRoot *root, const Params &p)
{
	bool ok = readStream(in, root, p);
	if (ok) return true; // new version success

	// rewind
	in.clear();
	in.seekg(0);

	// try reading osgb
	osg::ref_ptr<osg::Node> node = ModelSerializer::readOSGB(in, false, false);
	if (node == nullptr) return false;

	osg::ref_ptr<osg::Group> group = node->asGroup();
	// failed old version
	if (group == nullptr) return false; // old version fail
	// ok old version - try to convert
	root->loadOld(group);
	return true;
}

bool VSimSerializer::readStream(std::istream & in, VSimRoot *root, const Params &p)
{
	// read header
	// V S I M x x x x
	char header[8];
	in.read(header, 8);
	if (!in) return false;
	bool neq = strncmp(header, "VSIM", 4);
	if (neq) return false;

	// flatbuffer buffer
	std::string buffer;

	// read in flatbuffer size
	flatbuffers::uoffset_t fb_size;
	Util::appendn(in, buffer, sizeof(fb_size));
	fb_size = flatbuffers::ReadScalar<flatbuffers::uoffset_t>(buffer.c_str());

	// read the rest
	Util::appendn(in, buffer, fb_size);

	// construct & validate flatbuffer
	// TODO: flatbuffers should eventually have VerifySizePrefixedRoot
	auto *fb_root = flatbuffers::GetSizePrefixedRoot<fb::Root>(buffer.c_str());
	const uint8_t *buf =
		reinterpret_cast<const uint8_t *>(buffer.c_str());

	auto verifier = flatbuffers::Verifier(buf, buffer.size());
	bool fb_ok = verifier.VerifySizePrefixedBuffer<fb::Root>(fb::RootIdentifier());
	if (!fb_ok) return false;

	// now do business
	readRoot(fb_root, root, p);

	// read models
	ModelSerializer::readModels(fb_root->models(), root->models(), in, p.new_base);

	return true;
}

bool VSimSerializer::writeStream(std::ostream & out, const VSimRoot * root, const Params &p)
{
	// read header
	// v s i m x x x x
	char header[8] = { 'V', 'S', 'I', 'M', '\0', '\0', '\0', '\0' };
	out.write(header, 8);

	// flatbuffer buffer
	std::stringstream model_buffer;

	// build flatbuffer
	flatbuffers::FlatBufferBuilder builder;
	auto o_root = createRoot(&builder, root, model_buffer, p);
	builder.FinishSizePrefixed(o_root, fb::RootIdentifier());
	// TODO: when flatbuffer updates - fb::FinishRootSizePrefixed();

	auto verifier = flatbuffers::Verifier(builder.GetBufferPointer(), builder.GetSize());
	bool ok = verifier.VerifySizePrefixedBuffer<fb::Root>(fb::RootIdentifier());

	//const fb::Root *fb_root = flatbuffers::GetRoot<fb::Root>(builder.GetBufferPointer());

	// write flatbuffer
	const char *buf = reinterpret_cast<const char*>(builder.GetBufferPointer());
	size_t size = builder.GetSize(); // TODO: check this? does it include the prefix?
	out.write(buf, size);

	// write models
	out << model_buffer.rdbuf();

	return true;
}

void VSimSerializer::readRoot(const VSim::FlatBuffers::Root *buffer, VSimRoot *root, const Params &p)
{
	NarrativeGroup *nars = root->narratives();
	EResourceGroup *ers = root->resources();

	std::string version;
	if (buffer->version()) {
		version = buffer->version()->str();
	}
	if (buffer->narratives()) {
		NarrativeSerializer::readNarrativeTable(buffer->narratives(), nars);
	}
	if (buffer->eresources()) {
		ERSerializer::readERTable(buffer->eresources(), ers);
	}
	if (buffer->settings()) {
		buffer->settings()->UnPackTo(root->settings());
	}
	if (buffer->branding()) {
		CanvasSerializer::readCanvas(buffer->branding(), root->branding());
	}
}

flatbuffers::Offset<VSim::FlatBuffers::Root> VSimSerializer::createRoot(
	flatbuffers::FlatBufferBuilder * builder, const VSimRoot * root,
	std::ostream &model_data,
	const Params &p)
{
	auto o_version = builder->CreateString("2.0.0");
	auto o_nars = NarrativeSerializer::createNarrativeTable(builder, root->narratives());
	auto o_ers = ERSerializer::createERTable(builder, root->resources());
	auto o_models = ModelSerializer::createModels(builder, root->models(), model_data,
		p.old_base, p.new_base);
	auto o_settings = fb::CreateSettings(*builder, root->settings());
	auto o_branding = CanvasSerializer::createCanvas(builder, root->branding());

	fb::RootBuilder b_root(*builder);
	b_root.add_version(o_version);
	b_root.add_narratives(o_nars);
	b_root.add_eresources(o_ers);
	b_root.add_models(o_models);
	b_root.add_settings(o_settings);
	b_root.add_branding(o_branding);
	auto o_root = b_root.Finish();

	return o_root;
}


bool VSimSerializer::readVSimFile(const std::string & path, VSimRoot * root, const Params &p)
{
	std::ifstream in(path, std::ios::binary);
	if (!in.good()) {
		return false;
	}
	return VSimSerializer::readStreamRobust(in, root, p);
}

bool VSimSerializer::writeVSimFile(const std::string & path, const VSimRoot * root, const Params &p)
{
	std::ofstream out(path, std::ios::binary);
	if (!out.good()) {
		return false;
	}
	return VSimSerializer::writeStream(out, root, p);
}

bool VSimSerializer::importModel(const std::string & path, VSimRoot * root)
{
	return false;
}

bool VSimSerializer::importNarrativesStream(std::istream &in, NarrativeGroup *group)
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
	osg::ref_ptr<osg::Node> node = ModelSerializer::readOSGB(ss, false, false);
	if (!node) {
		qWarning() << "failed to load narrative file, not flatbuffer or osg";
		return false;
	}
	// try group, try narrative
	NarrativeOld *old = dynamic_cast<NarrativeOld*>(node.get());
	if (old) {
		auto nar = std::make_shared<Narrative>();
		nar->loadOld(old);
		group->append(nar);
		return true;
	}
	return false;
}

bool VSimSerializer::exportNarrativesStream(std::ostream &out, const NarrativeGroup *group,
	const std::set<size_t> &selection)
{
	if (!out.good()) {
		return false;
	}

	// make a new group w/ copy of narratives
	NarrativeGroup g;
	for (auto i : selection) {
		auto nar = group->childShared(i);
		if (!nar) continue;
		g.append(nar);
	}

	flatbuffers::FlatBufferBuilder builder;
	auto o_table = NarrativeSerializer::createNarrativeTable(&builder, &g);
	fb::FinishNarrativeTableBuffer(builder, o_table);

	const char *buf = reinterpret_cast<const char*>(builder.GetBufferPointer());
	out.write(buf, builder.GetSize());

	return out.good();
}

bool VSimSerializer::importEResources(std::istream &in, EResourceGroup *group)
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
	osg::ref_ptr<osg::Node> node = ModelSerializer::readOSGB(ss, false, false);
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

bool VSimSerializer::exportEResources(std::ostream &out, const EResourceGroup * group,
	const std::set<size_t>& selection)
{
	if (!out.good()) {
		return false;
	}

	// make a new group w/ copy of narratives
	EResourceGroup g;
	std::unordered_set<ECategory*> cats_added; // already has 
	for (auto i : selection) {
		auto res = group->childShared(i);
		if (!res) continue;

		g.append(res);

		auto cat = res->categoryShared();
		if (cat && (cats_added.find(cat.get()) == cats_added.end())) {
			cats_added.insert(cat.get());
			g.categories()->append(cat);
		}
	}

	flatbuffers::FlatBufferBuilder builder;
	auto o_table = ERSerializer::createERTable(&builder, &g);
	fb::FinishERTableBuffer(builder, o_table);

	const char *buf = reinterpret_cast<const char*>(builder.GetBufferPointer());
	out.write(buf, builder.GetSize());

	return out.good();
}
