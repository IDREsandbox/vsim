#ifndef VSIMSERIALIZER_H
#define VSIMSERIALIZER_H

#include <flatbuffers/flatbuffers.h>

#include "types_generated.h"
#include "eresources_generated.h"
#include "narratives_generated.h"
#include "settings_generated.h"
#include "vsim_generated.h"
#include <osg/Node>

class ModelGroup;
class VSimRoot;
class VSimApp;

namespace VSimSerializer {
	// read osg stream, binary or ascii

	// full compatible version
	// reads new and old version vsim
	bool readStreamRobust(std::istream &in, VSimRoot *root);

	// new version
	// attempts to read the new vsim format and initialize root
	// returns false if root left alone (full error, header read error)
	// returns true if root changed (success, partial error)
	bool readStream(std::istream &in, VSimRoot *root);
	bool writeStream(std::ostream &out, const VSimRoot *root);

	// osg stuff
	osg::ref_ptr<osg::Node> readOSGB(std::istream &in, bool ascii, bool zlib);
	size_t writeOSGB(std::ostream &out, osg::Node *node, bool ascii, bool zlib);

	// read in models from stream after reading table
	void readModels(const VSim::FlatBuffers::ModelTable *buffer,
		ModelGroup *models, std::istream &in);

	// create model options table
	// writes out model data at the same time
	flatbuffers::Offset<VSim::FlatBuffers::ModelTable>
		createModels(flatbuffers::FlatBufferBuilder *builder,
			const ModelGroup *models, std::ostream &model_data);
	// write model data
	//void writeModelsStream(const VSim::FlatBuffers::ModelTable *model_table,
	//	const ModelGroup *models, std::ostream &out);

	// read vsim flatbuffer
	// - doesn't read model data
	void readRoot(const VSim::FlatBuffers::Root *buffer,
		VSimRoot *root);

	// write vsim flatbuffer
	// - also writes model_data to stream. This is needed here because
	//   we need to write model metadata now, but don't know model sizes yet
	flatbuffers::Offset<VSim::FlatBuffers::Root>
		createRoot(flatbuffers::FlatBufferBuilder *builder,
			const VSimRoot *root, std::ostream &model_data);
}

//void readEResource();
#endif