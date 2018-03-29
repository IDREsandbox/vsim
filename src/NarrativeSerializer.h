#ifndef NARRATIVESEARIALIZER_H
#define NARRATIVESEARIALIZER_H

#include <flatbuffers/flatbuffers.h>
#include "types_generated.h"
#include "narratives_generated.h"

class NarrativeGroup;
class LabelStyleGroup;

namespace VSim {
namespace FlatBuffers {
	struct NarrativeTable;
}
}

namespace NarrativeSerializer {
	void readNarrativeTable(const VSim::FlatBuffers::NarrativeTable *buffer, NarrativeGroup *group);
	flatbuffers::Offset<VSim::FlatBuffers::NarrativeTable>
		createNarrativeTable(flatbuffers::FlatBufferBuilder *builder, const NarrativeGroup *group);

	void readStyleTable(const VSim::FlatBuffers::StyleTable *buffer, LabelStyleGroup *group);
	flatbuffers::Offset<VSim::FlatBuffers::StyleTable>
		createStyleTable(flatbuffers::FlatBufferBuilder *builder, const LabelStyleGroup *group);

}

#endif