#ifndef CANVASSERIALIZER_H
#define CANVASSERIALIZER_H

#include <flatbuffers/flatbuffers.h>
#include "types_generated.h"
#include "canvas_generated.h"

class CanvasItem;
class CanvasScene;
class LabelStyleGroup;
class LabelStyle;

// namespace VSim {
// namespace FlatBuffers {
// 	struct NarrativeTable;
// 	struct NarrativeSlide;
// 	struct StyleTable;
// }
// }

namespace CanvasSerializer {

	std::shared_ptr<CanvasItem>	readCanvasItem(
		const void *fb_item,
		const VSim::FlatBuffers::CanvasItem fb_type);
	bool createCanvasItem(flatbuffers::FlatBufferBuilder *builder,
		const CanvasItem *item,
		flatbuffers::Offset<void> *out_offset,
		VSim::FlatBuffers::CanvasItem *out_type);

	void readCanvasItems(
		const flatbuffers::Vector<flatbuffers::Offset<void>> *items,
		const flatbuffers::Vector<uint8_t> *types,
		CanvasScene *scene);
	void readCanvas(const VSim::FlatBuffers::Canvas *buffer,
		CanvasScene *scene);
	flatbuffers::Offset<VSim::FlatBuffers::Canvas> createCanvas(
		flatbuffers::FlatBufferBuilder *builder,
		const CanvasScene *scene);

	void readStyleTable(const VSim::FlatBuffers::StyleTable *buffer,
		LabelStyleGroup *group);
	flatbuffers::Offset<VSim::FlatBuffers::StyleTable>
		createStyleTable(flatbuffers::FlatBufferBuilder *builder,
			const LabelStyleGroup *group);

	void readLabelStyle(const VSim::FlatBuffers::LabelStyle *buffer,
		LabelStyle *style);
	flatbuffers::Offset<VSim::FlatBuffers::LabelStyle>
		createLabelStyle(flatbuffers::FlatBufferBuilder *builder,
			const LabelStyle *style);
}

#endif

