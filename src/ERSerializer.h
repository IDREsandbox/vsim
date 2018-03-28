#ifndef ERSERIALIZER_H
#define ERSERIALIZER_H

#include <flatbuffers/flatbuffers.h>
#include "types_generated.h"
#include "eresources_generated.h"

class EResourceGroup;

namespace VSim {
namespace FlatBuffers {
	struct ERTable;
}
}

namespace ERSerializer {
	void readERTable(const VSim::FlatBuffers::ERTable *buffer, EResourceGroup *group);
	flatbuffers::Offset<VSim::FlatBuffers::ERTable> createERTable(const EResourceGroup *group, flatbuffers::FlatBufferBuilder *builder);
}

//void readEResource();
#endif