#include "NarrativeSerializer.h"

#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeSlideItem.h"
#include "narrative/NarrativeSlideLabel.h"
#include "LabelStyleGroup.h"
#include "LabelStyle.h"
#include "TypesSerializer.h"

#include <QDebug>

namespace fb = VSim::FlatBuffers;

fb::VAlign qt2fbVAlign(Qt::Alignment al) {
	int i = al & Qt::AlignVertical_Mask;
	return static_cast<fb::VAlign>(i);
}

Qt::Alignment fb2qtVAlign(fb::HAlign al) {
	return static_cast<int>(al);
}

flatbuffers::Offset<fb::FillStyle>
	createFillStyle(flatbuffers::FlatBufferBuilder *builder, const QColor &bgcolor)
{
	fb::FillStyleBuilder b_fill(*builder);
	auto fcolor = TypesSerializer::qt2fbColor(bgcolor);
	b_fill.add_color(&fcolor);
	return b_fill.Finish();

}

void NarrativeSerializer::readNarrativeTable(
	const fb::NarrativeTable *buffer, NarrativeGroup *group)
{
	auto fb_nars = buffer->narratives();
	if (fb_nars) {
		for (auto fb_nar : *fb_nars) {
			if (!fb_nar) continue;

			auto nar = std::make_shared<Narrative>();

			if (fb_nar->styles()) readStyleTable(fb_nar->styles(), nar->labelStyles());
			if (fb_nar->title()) nar->setTitle(fb_nar->title()->str());
			if (fb_nar->author()) nar->setAuthor(fb_nar->author()->str());
			if (fb_nar->description()) nar->setDescription(fb_nar->description()->str());
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
				readStyleTable(fb_styles, nar->labelStyles());
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
		auto o_styles = createStyleTable(builder, nar->labelStyles());

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

		fb::NarrativeBuilder b_nar(*builder);
		b_nar.add_title(o_title);
		b_nar.add_author(o_auth);
		b_nar.add_description(o_desc);
		b_nar.add_styles(o_styles);
		b_nar.add_slides(o_slides);
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
	NarrativeSlide * slide)
{	

	if (buffer->camera()) slide->setCameraMatrix(
		TypesSerializer::fb2osgCameraMatrix(*buffer->camera()));
	slide->setDuration(buffer->duration());
	slide->setTransitionDuration(buffer->transition_duration());
	slide->setStayOnNode(buffer->wait_for_click());

	// items
	auto fb_items = buffer->items();
	if (fb_items) {
		for (size_t i = 0; i < fb_items->size(); i++) {
			auto type = (*buffer->items_type())[i];
			const void* ptr = (*fb_items)[i];
			if (!ptr) continue;

			if (type == fb::CanvasItem_CanvasLabel) {
				auto fb_label = static_cast<const fb::CanvasLabel *>(ptr);

				auto label = std::make_shared<NarrativeSlideLabel>();

				if (fb_label->html()) label->setHtml(fb_label->html()->str());
				label->setType(static_cast<LabelType>(fb_label->type()));
				label->setVAlignInt(fb_label->valign());

				// fill
				auto fs = fb_label->fill_style();
				if (fs) {
					if (fs->color()) label->setBackground(TypesSerializer::fb2qtColor(*fs->color()));
				}
				// rect
				auto fb_r = fb_label->rect();
				if (fb_r) {
					QRectF r(fb_r->x(), fb_r->y(), fb_r->w(), fb_r->h());
					label->setRect(r);
				}

				slide->append(label);
			}
			else if (type == fb::CanvasItem_CanvasImage) {
				// TODO
			}
		}
	}
}

flatbuffers::Offset<fb::Slide>
	NarrativeSerializer::createNarrativeSlide(flatbuffers::FlatBufferBuilder * builder,
		const NarrativeSlide * slide)
{
	// build canvas items
	std::vector<uint8_t> v_item_types;
	std::vector<flatbuffers::Offset<void>> v_items;
	for (uint i = 0; i < slide->size(); i++) {
		NarrativeSlideLabel *label = dynamic_cast<NarrativeSlideLabel*>(slide->child(i));
		if (!label) continue;
		auto o_html = builder->CreateString(label->getHtml());
		auto o_fill = createFillStyle(builder, label->getBackground());

		QRectF qr = label->getRect();
		fb::Rect rect(qr.x(), qr.y(), qr.width(), qr.height());

		fb::CanvasLabelBuilder b_label(*builder);
		b_label.add_html(o_html);
		b_label.add_type(static_cast<fb::LabelType>(label->getStyleTypeInt()));
		b_label.add_valign(qt2fbVAlign(label->getVAlign()));
		b_label.add_fill_style(o_fill);
		b_label.add_rect(&rect);
		auto o_label = b_label.Finish();

		v_item_types.push_back(fb::CanvasItem::CanvasItem_CanvasLabel);
		v_items.push_back(o_label.Union());
	} // end items
	auto o_item_types = builder->CreateVector(v_item_types);
	auto o_items = builder->CreateVector(v_items);

	fb::SlideBuilder b_slide(*builder);
	fb::CameraPosDirUp camera = TypesSerializer::osg2fbCameraMatrix(slide->getCameraMatrix());
	b_slide.add_camera(&camera);
	b_slide.add_duration(slide->getDuration());
	b_slide.add_wait_for_click(slide->getStayOnNode());
	b_slide.add_transition_duration(slide->getTransitionDuration());
	b_slide.add_items_type(o_item_types);
	b_slide.add_items(o_items);
	auto o_slide = b_slide.Finish();

	return o_slide;
}

void NarrativeSerializer::readLabelStyle(
	const fb::LabelStyle *buffer, LabelStyle *style)
{
	// fill style
	auto fs = buffer->fill_style();
	if (fs) {
		if (fs->color()) style->m_bg_color = TypesSerializer::fb2qtColor(*fs->color());
	}
	// text style
	auto ts = buffer->text_style();
	if (ts) {
		if (ts->font_family()) style->m_font_family = ts->font_family()->str();
		if (ts->foreground()) style->m_fg_color = TypesSerializer::fb2qtColor(*ts->foreground());
		style->m_margin = ts->margin();
		style->m_point_size = ts->point_size();
		style->m_weight = ts->weight();
		style->m_underline = ts->underlined();
		style->m_ital = ts->italicized();
		style->setHAlign(static_cast<Qt::Alignment>(ts->halign()));
		style->setVAlign(static_cast<Qt::Alignment>(ts->valign()));
	}
}

flatbuffers::Offset<fb::LabelStyle>
	NarrativeSerializer::createLabelStyle(
		flatbuffers::FlatBufferBuilder *builder, const LabelStyle *style)
{
	auto o_fill = createFillStyle(builder, style->m_bg_color);

	auto o_font = builder->CreateString(style->m_font_family);

	auto b_text = fb::TextStyleBuilder(*builder);
	b_text.add_font_family(o_font);
	auto fgcolor = TypesSerializer::qt2fbColor(style->m_fg_color);
	b_text.add_foreground(&fgcolor);
	int halign = style->m_align & Qt::AlignHorizontal_Mask;
	b_text.add_halign(static_cast<fb::HAlign>(halign));
	b_text.add_italicized(style->getItalicized());
	b_text.add_margin(style->getMargin());
	b_text.add_point_size(style->getPointSize());
	b_text.add_weight(style->getWeight());
	int valign = style->m_align & Qt::AlignVertical_Mask;
	b_text.add_valign(static_cast<fb::VAlign>(valign));
	b_text.add_underlined(style->getUnderline());
	auto o_text = b_text.Finish();

	fb::LabelStyleBuilder b_style(*builder);
	b_style.add_fill_style(o_fill);
	b_style.add_text_style(o_text);
	//b_style.add_line_style();
	b_style.add_type(static_cast<fb::LabelType>(style->getType()));
	auto o_style = b_style.Finish();

	return o_style;
}

void NarrativeSerializer::readStyleTable(const fb::StyleTable *buffer,
	LabelStyleGroup * group)
{
	auto h1 = buffer->header1();
	if (h1) readLabelStyle(h1, group->getStyle(LabelType::HEADER1));
	auto h2 = buffer->header2();
	if (h2) readLabelStyle(h2, group->getStyle(LabelType::HEADER2));
	auto label = buffer->label();
	if (label) readLabelStyle(label, group->getStyle(LabelType::LABEL));
	auto body = buffer->body();
	if (body) readLabelStyle(body, group->getStyle(LabelType::BODY));
}

flatbuffers::Offset<fb::StyleTable>
	NarrativeSerializer::createStyleTable(
		flatbuffers::FlatBufferBuilder * builder, const LabelStyleGroup * group)
{
	LabelStyle *body = group->getStyle(LabelType::BODY);

	auto o_header1 = createLabelStyle(builder, group->getStyle(LabelType::HEADER1));
	auto o_header2 = createLabelStyle(builder, group->getStyle(LabelType::HEADER2));
	auto o_label = createLabelStyle(builder, group->getStyle(LabelType::LABEL));
	auto o_body = createLabelStyle(builder, group->getStyle(LabelType::BODY));

	fb::StyleTableBuilder b_styles(*builder);
	b_styles.add_header1(o_header1);
	b_styles.add_header2(o_header2);
	b_styles.add_label(o_label);
	b_styles.add_body(o_body);
	return b_styles.Finish();
}
