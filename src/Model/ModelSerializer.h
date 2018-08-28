#ifndef MODELSERIALIZER_H
#define MODELSERIALIZER_H

#include <flatbuffers/flatbuffers.h>
#include <fstream>
#include <vector>
#include <osg/Node>
#include <QString>
#include <osg/NodeVisitor>
#include <stack>

#include "model_generated.h"
#include "Core/TypesSerializer.h"

class ModelGroup;

namespace ModelSerializer {
	osg::ref_ptr<osg::Node> readOSGB(std::istream & in, bool ascii, bool zlib);
	size_t writeOSGB(std::ostream & out, osg::Node * node, bool ascii, bool zlib);

	// read in models from stream after reading table
	// ModelTable has the metadata
	// istream has the raw data
	// base_dir is what relative paths are based on
	void readModels(const VSim::FlatBuffers::ModelTable *buffer,
		ModelGroup *models,
		std::istream &in,
		const QString &base_dir);

	// create model options table
	// writes out model data at the same time
	// old_base and new_base make relative paths to be valid 
	flatbuffers::Offset<VSim::FlatBuffers::ModelTable>
		createModels(flatbuffers::FlatBufferBuilder *builder,
			const ModelGroup *models,
			std::ostream &model_data,
			const TypesSerializer::Params &p);

	// does osg readNodeFile
	// also removes ProxyNodes
	osg::ref_ptr<osg::Node> readNodeFile(const std::string &filename);

	class ExternalReferenceVisitor : public osg::NodeVisitor {
	public:
		ExternalReferenceVisitor();
		void apply(osg::ProxyNode& node) override;
		static void visit(osg::ref_ptr<osg::Node> node);

	private:
		std::map<std::string, osg::ref_ptr<osg::Node>> m_node_map;
		std::stack<std::string> m_path_stack;
	};

	class ExternalReferenceDebugger : public osg::NodeVisitor {
	public:
		ExternalReferenceDebugger();
		void doNothing(bool do_nothing);
		void apply(osg::ProxyNode& node) override;
	private:
		bool m_do_nothing;
	};
};

#endif