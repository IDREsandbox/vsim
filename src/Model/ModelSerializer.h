#ifndef MODELSERIALIZER_H
#define MODELSERIALIZER_H

#include <flatbuffers/flatbuffers.h>
#include <fstream>
#include <vector>
#include <osg/Node>
#include <QString>

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

	//void readEmbeddedModels(std::istream &stream, ModelGroup *group);
	//void loadLinkedModels(ModelGroup *group);
	//// returns byte sizes of each model written, in order
	//std::vector<size_t> writeEmbeddedModels(std::ostream &stream, ModelGroup *group);
};

#endif