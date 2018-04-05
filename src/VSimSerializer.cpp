#include "VSimSerializer.h"
#include "NarrativeSerializer.h"
#include "ERSerializer.h"
#include "VSimRoot.h"
#include "ModelGroup.h"
#include <flatbuffers/flatbuffers.h>
#include <sstream>
#include <iterator>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

namespace fb = VSim::FlatBuffers;

void appendn(std::istream &in, std::string &buff, size_t n) {
	// doesn't work
	// buff.reserve(buff.size() + n);
	// std::copy_n(std::istreambuf_iterator<char>(in), n, std::back_inserter(buff));
	std::string s(n, '\0');
	in.read(&s[0], n);
	buff.append(s);
}

bool VSimSerializer::readStreamRobust(std::istream & in, VSimRoot *root)
{
	bool ok = readStream(in, root);
	if (ok) return true; // new version success

	// rewind
	in.clear();
	in.seekg(0);

	// try reading osgb
	osg::ref_ptr<osg::Node> node = readOSGB(in, false, false);
	if (node == nullptr) return false;

	osg::ref_ptr<osg::Group> group = node->asGroup();
	// failed old version
	if (group == nullptr) return false; // old version fail
	// ok old version - try to convert
	root->loadOld(group);
	return true;
}

bool VSimSerializer::readStream(std::istream & in, VSimRoot *root)
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
	appendn(in, buffer, sizeof(fb_size));
	fb_size = flatbuffers::ReadScalar<flatbuffers::uoffset_t>(buffer.c_str());

	// read the rest
	appendn(in, buffer, fb_size);

	// construct & validate flatbuffer
	// TODO: flatbuffers should eventually have VerifySizePrefixedRoot
	auto *fb_root = flatbuffers::GetSizePrefixedRoot<fb::Root>(buffer.c_str());
	const uint8_t *buf =
		reinterpret_cast<const uint8_t *>(buffer.c_str());

	auto verifier = flatbuffers::Verifier(buf, buffer.size());
	bool fb_ok = verifier.VerifySizePrefixedBuffer<fb::Root>(fb::RootIdentifier());
	if (!fb_ok) return false;

	// now do business
	readRoot(fb_root, root);

	// read models
	if (fb_root->models()) readModels(fb_root->models(), root->models(), in);

	return true;
}

bool VSimSerializer::writeStream(std::ostream & out, VSimRoot * root)
{
	// read header
	// v s i m x x x x
	char header[8] = { 'V', 'S', 'I', 'M', '\0', '\0', '\0', '\0' };
	out.write(header, 8);

	// flatbuffer buffer
	std::stringstream model_buffer;

	// build flatbuffer
	flatbuffers::FlatBufferBuilder builder;
	auto o_root = createRoot(&builder, root, model_buffer);
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

osg::ref_ptr<osg::Node> VSimSerializer::readOSGB(std::istream & in, bool ascii, bool zlib)
{
	osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
	if (ascii) options->setPluginStringData("fileType", "Ascii");
	if (zlib) options->setPluginStringData("Compressor", "zlib");
	options->setPluginStringData("WriteImageHint", "IncludeData");

	osg::ref_ptr<osg::Node> node;
	if (!rw) {
		//QMessageBox::warning(m_window, "Load Error", "Error creating osgb reader " + QString::fromStdString(filename));
		qWarning() << "Error creating osgb reader";
		return nullptr;
	}
	osgDB::ReaderWriter::ReadResult result = rw->readNode(in, options);
	if (result.success()) {
		node = result.takeNode();
	}
	else {
		//QMessageBox::warning(m_window, "Load Error", "Error converting file to osg nodes " + QString::fromStdString(filename));
		qWarning() << "Error reading node";
		return nullptr;
	}
	return node;
}

size_t VSimSerializer::writeOSGB(std::ostream & out, osg::Node * node, bool ascii, bool zlib)
{
	osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
	if (ascii) options->setPluginStringData("fileType", "Ascii");
	if (zlib) options->setPluginStringData("Compressor", "zlib");
	options->setPluginStringData("WriteImageHint", "IncludeData");

	size_t before = out.tellp();
	if (!rw) {
		qWarning() << "Error creating osgb writer";
		return 0;
	}
	auto result = rw->writeNode(*node, out, options);
	if (result.success()) {
	}
	else {
		qWarning() << "Error writing osg nodes";
		return 0;
	}
	size_t after = out.tellp();
	size_t length = after - before;
	return length; // apparently osg doesn't return sizes...
}

void VSimSerializer::readModels(const VSim::FlatBuffers::ModelTable *buffer,
	ModelGroup *models, std::istream &in)
{
	auto v_models = buffer->models();
	if (!v_models) return;
	for (auto fb_model : *v_models) {
		bool ascii = fb_model->ascii();
		bool zlib = fb_model->zlib();
		std::string format = fb_model->format()->str();
		size_t size = fb_model->size();

		// read in size bytes, to use as a size limit
		std::string buffer;
		appendn(in, buffer, size);
		std::stringstream ss;
		ss << buffer;

		osg::ref_ptr<osg::Node> model;
		// read vsim
		if (format == "osgb") {
			model = readOSGB(ss, ascii, zlib);
		}

		models->addChild(model);
	}
}

flatbuffers::Offset<fb::ModelTable>
	VSimSerializer::createModels(flatbuffers::FlatBufferBuilder *builder,
		const ModelGroup *model_group, std::ostream &model_data)
{
	// build models
	std::vector<flatbuffers::Offset<fb::Model>> v_models;
	for (size_t i = 0; i < model_group->getNumChildren(); i++) {
		auto o_format = builder->CreateString("osgb");

		bool ascii = false;
		bool zlib = true;

		// write osg stuff
		osg::Node *node = model_group->child(i);
		if (!node) continue;
		size_t size = writeOSGB(model_data, node, ascii, zlib);

		fb::ModelBuilder b_model(*builder);
		b_model.add_ascii(ascii);
		b_model.add_zlib(zlib);
		b_model.add_size(size);
		b_model.add_format(o_format);
		auto o_model = b_model.Finish();
		v_models.push_back(o_model);
	}
	auto o_models = builder->CreateVector(v_models);

	fb::ModelTableBuilder b_table(*builder);
	b_table.add_models(o_models);
	return b_table.Finish();
}

void VSimSerializer::readRoot(const VSim::FlatBuffers::Root *buffer, VSimRoot *root)
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
}

flatbuffers::Offset<VSim::FlatBuffers::Root> VSimSerializer::createRoot(
	flatbuffers::FlatBufferBuilder * builder, const VSimRoot * root,
	std::ostream &model_data)
{
	auto o_version = builder->CreateString("2.0.0");
	auto o_nars = NarrativeSerializer::createNarrativeTable(builder, root->narratives());
	auto o_ers = ERSerializer::createERTable(builder, root->resources());
	auto o_models = createModels(builder, root->models(), model_data);
	auto o_settings = fb::CreateSettings(*builder, root->settings());

	fb::RootBuilder b_root(*builder);
	b_root.add_version(o_version);
	b_root.add_narratives(o_nars);
	b_root.add_eresources(o_ers);
	b_root.add_models(o_models);
	b_root.add_settings(o_settings);
	auto o_root = b_root.Finish();

	return o_root;
}
