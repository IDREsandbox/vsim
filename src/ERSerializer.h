#ifndef ERSERIALIZER_H
#define ERSERIALIZER_H

#include <flatbuffers/flatbuffers.h>
#include "types_generated.h"
#include "eresources_generated.h"
#include "Core/TypesSerializer.h"

class EResourceGroup;

namespace VSim {
namespace FlatBuffers {
	struct ERTable;
}
}

namespace ERSerializer {
	void readERTable(const VSim::FlatBuffers::ERTable *buffer,
		EResourceGroup *group,
		const TypesSerializer::Params &p);
	flatbuffers::Offset<VSim::FlatBuffers::ERTable> createERTable(
		flatbuffers::FlatBufferBuilder *builder,
		const EResourceGroup *group,
		const TypesSerializer::Params &p);
}

//void readEResource();
#endif