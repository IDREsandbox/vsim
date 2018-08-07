#include "ModelSerializer.h"

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <QDebug>

#include "Model/ModelGroup.h"
#include "Model/Model.h"
#include "Core/Util.h"
#include "Core/TypesSerializer.h"

namespace fb = VSim::FlatBuffers;

osg::ref_ptr<osg::Node> ModelSerializer::readOSGB(std::istream & in, bool ascii, bool zlib)
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

size_t ModelSerializer::writeOSGB(std::ostream & out, osg::Node * node, bool ascii, bool zlib)
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

void ModelSerializer::readModels(const VSim::FlatBuffers::ModelTable *buffer,
	ModelGroup *models, std::istream &in, const QString &base_dir)
{
	if (!buffer) return;
	auto v_models = buffer->models();
	if (!v_models) return;
	for (auto fb_model : *v_models) {
		auto model = std::make_shared<Model>();

		bool ascii = fb_model->ascii();
		bool zlib = fb_model->zlib();
		size_t size = fb_model->size();
		bool external = fb_model->external();

		if (fb_model->name()) model->setName(fb_model->name()->str());

		QString path;
		if (fb_model->path()) {
			path = QString::fromStdString(fb_model->path()->str());
		}
		QString full_path = Util::resolvePath(path, base_dir);

		if (external) {
			// load external model
			osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(full_path.toStdString());
			model->setFile(path.toStdString(), node);
		}
		else {
			// read an embedded model from the stream
			std::string buffer;
			Util::appendn(in, buffer, size);
			std::stringstream ss;
			ss << buffer;

			std::string format;
			if (fb_model->format()) format = fb_model->format()->str();

			osg::ref_ptr<osg::Node> node;
			// read vsim
			if (format == "osgb") {
				node = readOSGB(ss, ascii, zlib);
			}
			if (!node) continue;

			model->embedModel(node);
		}

		// read in size bytes, to use as a size limit

		//model->setNode(node);
		//models->append(model);

		models->addModel(model);
	}
}

flatbuffers::Offset<fb::ModelTable>
ModelSerializer::createModels(flatbuffers::FlatBufferBuilder *builder,
	const ModelGroup *model_group,
	std::ostream &model_data,
	const TypesSerializer::Params &p
	)
{
	// build models
	std::vector<flatbuffers::Offset<fb::Model>> v_models;
	for (size_t i = 0; i < model_group->cgroup()->size(); i++) {
		auto model = model_group->cgroup()->child(i);

		bool embed = model->embedded();

		flatbuffers::Offset<flatbuffers::String> o_format, o_name, o_path;
		o_name = builder->CreateString(model->name());
		if (embed) {
			o_format = builder->CreateString("osgb");
		}
		else {
			TypesSerializer::createRelativePath(builder, QString::fromStdString(model->path()), p);
		}

		bool ascii = false;
		bool zlib = false;

		// write osg stuff
		osg::Node *node = model->node();
		if (!node) continue;
		size_t size = 0;
		if (model->embedded()) {
			size = writeOSGB(model_data, node, ascii, zlib);
		}

		fb::ModelBuilder b_model(*builder);
		b_model.add_name(o_name);
		b_model.add_external(!embed);
		if (embed) {
			b_model.add_ascii(ascii);
			b_model.add_zlib(zlib);
			b_model.add_size(size);
			b_model.add_format(o_format);
		}
		else {
			b_model.add_path(o_path);
		}
		auto o_model = b_model.Finish();
		v_models.push_back(o_model);
	}
	auto o_models = builder->CreateVector(v_models);

	fb::ModelTableBuilder b_table(*builder);
	b_table.add_models(o_models);
	return b_table.Finish();
}