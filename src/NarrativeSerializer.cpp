#include "NarrativeSerializer.h"

#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "narrative/NarrativeSlide.h"
#include "Canvas/LabelStyleGroup.h"
#include "Canvas/LabelStyle.h"
#include "Canvas/CanvasSerializer.h"
#include "Core/TypesSerializer.h"
#include "Core/LockTable.h"

#include <QDebug>

namespace fb = VSim::FlatBuffers;

void NarrativeSerializer::readNarrativeTable(
	const fb::NarrativeTable *buffer, NarrativeGroup *group)
{
	auto fb_nars = buffer->narratives();
	if (fb_nars) {
		for (auto fb_nar : *fb_nars) {
			if (!fb_nar) continue;

			auto nar = std::make_shared<Narrative>();

			if (fb_nar->title()) nar->setTitle(fb_nar->title()->str());
			if (fb_nar->author()) nar->setAuthor(fb_nar->author()->str());
			if (fb_nar->description()) nar->setDescription(fb_nar->description()->str());
			nar->lockTable()->readLockTable(fb_nar->lock_table());

			// slides
			auto fb_slides = fb_nar->slides();
			if (fb_slides) {
				for (auto fb_slide : *fb_slides) {
					if (!fb_slide) continue;

					auto slide = std::make_shared<NarrativeSlide>();
					readNarrativeSlide(fb_slide, slide.get());
					nar->append(slide);
				}
			}
			// styles
			auto fb_styles = fb_nar->styles();
			if (fb_styles) {
				CanvasSerializer::readStyleTable(fb_styles, nar->labelStyles());
			}

			group->append(nar);
		}
	}
}

flatbuffers::Offset<fb::NarrativeTable>
	NarrativeSerializer::createNarrativeTable(
		flatbuffers::FlatBufferBuilder *builder, const NarrativeGroup *group)
{
	// build narratives
	std::vector<flatbuffers::Offset<fb::Narrative>> v_nars;
	for (uint i = 0; i < group->size(); i++) {
		Narrative *nar = group->child(i);
		if (!nar) continue;

		// build styles
		auto o_styles = CanvasSerializer::createStyleTable(builder, nar->labelStyles());

		// build slides
		std::vector<flatbuffers::Offset<fb::Slide>> v_slides;
		for (uint i = 0; i < nar->size(); i++) {
			NarrativeSlide *slide = nar->child(i);
			auto o_slide = createNarrativeSlide(builder, slide);
			v_slides.push_back(o_slide);
		} // end slides
		auto o_slides = builder->CreateVector(v_slides);

		// narrative strings
		auto o_title = builder->CreateString(nar->getTitle());
		auto o_auth = builder->CreateString(nar->getAuthor());
		auto o_desc = builder->CreateString(nar->getDescription());

		// lock table
		auto o_lock = nar->lockTable()->createLockTable(builder);

		fb::NarrativeBuilder b_nar(*builder);
		b_nar.add_title(o_title);
		b_nar.add_author(o_auth);
		b_nar.add_description(o_desc);
		b_nar.add_styles(o_styles);
		b_nar.add_slides(o_slides);
		b_nar.add_lock_table(o_lock);
		auto o_nar = b_nar.Finish();

		v_nars.push_back(o_nar);
	} // end nars
	auto o_nars = builder->CreateVector(v_nars);

	fb::NarrativeTableBuilder b_table(*builder);
	b_table.add_narratives(o_nars);
	auto o_table = b_table.Finish();

	return o_table;
}

void NarrativeSerializer::readNarrativeSlide(const fb::Slide * buffer,
	NarrativeSlide *slide)
{	

	if (buffer->camera()) slide->setCameraMatrix(
		TypesSerializer::fb2osgCameraMatrix(*buffer->camera()));
	slide->setDuration(buffer->duration());
	slide->setTransitionDuration(buffer->transition_duration());
	slide->setStayOnNode(buffer->wait_for_click());

	// canvas items, deprecated
	auto fb_items = buffer->items();
	auto fb_types = buffer->items_type();
	CanvasSerializer::readCanvasItems(fb_items, fb_types, slide->scene());

	// canvas
	CanvasSerializer::readCanvas(buffer->canvas(), slide->scene());
}

flatbuffers::Offset<fb::Slide>
	NarrativeSerializer::createNarrativeSlide(flatbuffers::FlatBufferBuilder * builder,
		const NarrativeSlide * slide)
{
	auto o_canvas = CanvasSerializer::createCanvas(builder, slide->scene());
	auto o_item_types = builder->CreateVector<uint8_t>({}); // deprecated
	auto o_items = builder->CreateVector<flatbuffers::Offset<void>>({});

	fb::SlideBuilder b_slide(*builder);
	fb::CameraPosDirUp camera = TypesSerializer::osg2fbCameraMatrix(slide->getCameraMatrix());
	b_slide.add_camera(&camera);
	b_slide.add_duration(slide->getDuration());
	b_slide.add_wait_for_click(slide->getStayOnNode());
	b_slide.add_transition_duration(slide->getTransitionDuration());
	b_slide.add_items_type(o_item_types); // deprecated
	b_slide.add_items(o_items);
	b_slide.add_canvas(o_canvas);
	auto o_slide = b_slide.Finish();

	return o_slide;
}
