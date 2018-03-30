#ifndef VSIMSERIALIZER_H
#define VSIMSERIALIZER_H

#include <flatbuffers/flatbuffers.h>

#include "types_generated.h"
#include "eresources_generated.h"
#include "narratives_generated.h"
#include "vsim_generated.h"

class ModelGroup;
class VSimRoot;

namespace VSimSerializer {

	// read osg stream, binary or ascii

	// osg to fb::Model
	void readModel(const VSim::FlatBuffers::Model *buffer,
		ModelGroup *models);
	// fb::Model to osg
	flatbuffers::Offset<VSim::FlatBuffers::Model>
		createModel(flatbuffers::FlatBufferBuilder *builder,
			const ModelGroup *models);

	// read vsim flatbuffer
	void readRoot(const VSim::FlatBuffers::Root *buffer,
		VSimRoot *root);

	// write vsim flatbuffer
	flatbuffers::Offset<VSim::FlatBuffers::Root>
		createRoot(flatbuffers::FlatBufferBuilder *builder,
			const VSimRoot *root);
}

//void readEResource();
#endif