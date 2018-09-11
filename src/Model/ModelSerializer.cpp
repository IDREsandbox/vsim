#include "ModelSerializer.h"

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <QDebug>
#include <osg/ProxyNode>

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
	ExternalReferenceVisitor::visit(node);

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

		std::string name;
		if (fb_model->name()) name = fb_model->name()->str();
		model->setName(name);

		QString path;
		if (fb_model->path()) {
			path = QString::fromStdString(fb_model->path()->str());
		}
		QString full_path = Util::resolvePath(path, base_dir);

		if (external) {
			// load external model
			osg::ref_ptr<osg::Node> node = readNodeFile(full_path.toStdString());
			if (!node) {
				qInfo() << "external model" << name.c_str() << "failed to load, path:" << path;
			}
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
			o_path = TypesSerializer::createRelativePath(builder,
				QString::fromStdString(model->path()), p);
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

osg::ref_ptr<osg::Node> ModelSerializer::readNodeFile(const std::string & filename)
{
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename);
	if (node) {
		ExternalReferenceVisitor::visit(node);
	}
	return node;
}


ModelSerializer::ExternalReferenceVisitor::ExternalReferenceVisitor()
	: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
}

void ModelSerializer::ExternalReferenceVisitor::apply(osg::ProxyNode& node)
{
	// the old way, re-read the files and replace ProxyNodes with Groups

	// the new way, we should have already read in all of these files,
	//   so just copy the group contents to plain old groups
	// if u read the source, nodes are created proactively or lazily and
	//   traversal triggers node loading (I think?)
	// are duplicate models merged? I know for the old method they are.
	//   I tested it, yes they are usually merged


	// we can preorder or postorder traverse, does it matter?

	osg::ref_ptr<osg::Group> new_group(new osg::Group);

	// transfer children
	for (size_t i = 0; i < node.getNumChildren(); i++) {
		new_group->addChild(node.getChild(i));
	}
	node.removeChildren(0, node.getNumChildren());

	// replace this node in the parents
	auto pars = node.getParents();
	for (auto *par : pars) {
		par->replaceChild(&node, new_group);
	}

	// now continue on w/ the traversal
	traverse(*new_group);


	// fix us first? or fix our children first? does it matter? probably not?
	// traverse all children

	//OSG_WARN << "ProxyNode " << node.getName() << std::endl;
	//osg::Node::ParentList parents = node.getParents();
	//if (parents.size() > 1)
	//	assert(0);
	//osg::ref_ptr<osg::Group> parent = parents[0];
	//osg::ref_ptr<osg::Group> newgroup = new osg::Group();
	//int n = node.getNumFileNames();
	//for (int i = 0; i < n; i++)
	//{
	//	std::string filename = node.getFileName(i);
	//	OSG_WARN << i << ": " << filename << std::endl;
	//	std::string pathfilename = osgDB::findDataFile(filename);
	//	if (pathfilename == "")
	//	{
	//		OSG_WARN << "Not found." << std::endl;
	//	}
	//	else
	//	{
	//		NodeMap::iterator result = m_nodemap.find(pathfilename);
	//		if (result != m_nodemap.end())
	//		{
	//			OSG_WARN << "Duplicate hit" << std::endl;
	//			newgroup->addChild(result->second);
	//		}
	//		else
	//		{
	//			osg::ref_ptr<osg::Node> newnode = osgDB::readNodeFile(pathfilename);
	//			assert(newnode.get() != NULL);
	//			newgroup->addChild(newnode);
	//			InsertResult ir = m_nodemap.insert(MapPair(pathfilename, newnode));
	//			assert(ir.second == true);

	//			std::string path = osgDB::getFilePath(pathfilename);
	//			osgDB::FilePathList pl = osgDB::getDataFilePathList();
	//			osgDB::FilePathList pl2;
	//			pl2.push_back(path);
	//			osgDB::setDataFilePathList(pl2);
	//			OSG_WARN << "Entering child with path " << path << std::endl;
	//			traverse(*newnode);
	//			OSG_WARN << "Exiting child." << std::endl;
	//			osgDB::setDataFilePathList(pl);
	//		}
	//	}
	//}
	//parent->replaceChild(&node, newgroup);
	//traverse(*newgroup);
}

void ModelSerializer::ExternalReferenceVisitor::visit(osg::ref_ptr<osg::Node> node)
{
	if (!node) return;
	// do a dummy traversal, hopefully this triggers the lazy loading
	ExternalReferenceDebugger erd;
	erd.doNothing(true);
	node->accept(erd);

	ExternalReferenceVisitor visitor;
	visitor.setTraversalMask(0xffffffff);
	visitor.setNodeMaskOverride(0xffffffff);
	//visitor.setBaseDir();
	node->accept(visitor);

	//ProgressDialog pd;
	//pd.show("Reading external references. Please wait.");
	//std::string path = osgDB::getFilePath(m_modelFilename);
	//osgDB::FilePathList pl = osgDB::getDataFilePathList();
	//osgDB::FilePathList pl2;
	//pl2.push_back(path);
	//osgDB::setDataFilePathList(pl2); e
	//	ExternalReferenceVisitor erv;
	//erv.setTraversalMask(0xffffffff);
	//erv.setNodeMaskOverride(0xffffffff);
	//m_model->accept(erv);
	//osgDB::setDataFilePathList(pl);
	//pd.hide();
}

ModelSerializer::ExternalReferenceDebugger::ExternalReferenceDebugger()
	: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
	m_do_nothing(false)
{
	setTraversalMask(0xffffffff);
}

void ModelSerializer::ExternalReferenceDebugger::doNothing(bool do_nothing)
{
	m_do_nothing = do_nothing;
}

void ModelSerializer::ExternalReferenceDebugger::apply(osg::ProxyNode& node)
{
	if (m_do_nothing) return;

	qDebug() << "proxy node" << node.getName().c_str() << "type:" << node.getLoadingExternalReferenceMode()
		<< node.getNumFileNames() << node.getNumChildren() << "name" << node.getName().c_str();

	int n = node.getNumFileNames();
	for (int i = 0; i < n; i++)
	{
		std::string filename = node.getFileName(i);
		osg::Node *child;
		if ((size_t)i >= node.getNumChildren()) {
			child = nullptr;
		}
		else {
			child = node.getChild(i);
		}
		std::string name;
		if (child) name = child->getName();
		qDebug() << i << node.getFileName(i).c_str() << child << name.c_str();
	}

	traverse(node);
}

