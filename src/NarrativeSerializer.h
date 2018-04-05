#ifndef NARRATIVESEARIALIZER_H
#define NARRATIVESEARIALIZER_H

#include <flatbuffers/flatbuffers.h>
#include "types_generated.h"
#include "narratives_generated.h"

class NarrativeGroup;
class LabelStyleGroup;
class LabelStyle;
class NarrativeSlide;

namespace VSim {
namespace FlatBuffers {
	struct NarrativeTable;
	struct NarrativeSlide;
	struct StyleTable;
}
}

namespace NarrativeSerializer {
	// adds loaded narratives to group
	void readNarrativeTable(const VSim::FlatBuffers::NarrativeTable *buffer,
		NarrativeGroup *group);
	flatbuffers::Offset<VSim::FlatBuffers::NarrativeTable>
		createNarrativeTable(flatbuffers::FlatBufferBuilder *builder,
			const NarrativeGroup *group);

	void readNarrativeSlide(const VSim::FlatBuffers::Slide *buffer,
		NarrativeSlide *slide);
	flatbuffers::Offset<VSim::FlatBuffers::Slide>
		createNarrativeSlide(flatbuffers::FlatBufferBuilder *builder,
			const NarrativeSlide *slide);

	void readLabelStyle(const VSim::FlatBuffers::LabelStyle *buffer,
		LabelStyle *style);
	flatbuffers::Offset<VSim::FlatBuffers::LabelStyle>
		createLabelStyle(flatbuffers::FlatBufferBuilder *builder,
			const LabelStyle *style);

	void readStyleTable(const VSim::FlatBuffers::StyleTable *buffer,
		LabelStyleGroup *group);
	flatbuffers::Offset<VSim::FlatBuffers::StyleTable>
		createStyleTable(flatbuffers::FlatBufferBuilder *builder,
			const LabelStyleGroup *group);
}

#endif