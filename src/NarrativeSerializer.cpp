#include "NarrativeSerializer.h"

#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeSlideItem.h"
#include "narrative/NarrativeSlideLabel.h"
#include "LabelStyleGroup.h"
#include "LabelStyle.h"
#include "TypesSerializer.h"

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
	auto fcolor = Serializer::qt2fbColor(bgcolor);
	b_fill.add_color(&fcolor);
	b_fill.Finish();
}

void NarrativeSerializer::readNarrativeTable(
	const fb::NarrativeTable *buffer, NarrativeGroup *group)
{

}

flatbuffers::Offset<fb::NarrativeTable>
	NarrativeSerializer::createNarrativeTable(
		flatbuffers::FlatBufferBuilder *builder, const NarrativeGroup *group)
{
	// build narratives
	std::vector<flatbuffers::Offset<fb::Narrative>> v_nars;
	for (uint i = 0; i < group->getNumChildren(); i++) {
		Narrative2 *nar = dynamic_cast<Narrative2*>(group->child(i));
		if (!nar) continue;

		// build styles
		auto o_styles = createStyleTable(builder, nar->getLabelStyles());

		// build slides
		std::vector<flatbuffers::Offset<fb::Slide>> v_slides;
		for (uint i = 0; i < nar->getNumChildren(); i++) {
			NarrativeSlide *slide = dynamic_cast<NarrativeSlide*>(nar->child(i));

			// build canvas items
			auto o_items = createStyleTable(builder, nar->getLabelStyles());
			for (uint i = 0; i < slide->getNumChildren(); i++) {
				NarrativeSlideLabel *label = dynamic_cast<NarrativeSlideLabel*>(slide->child(i));
				if (!label) continue;

				auto o_html = builder->CreateString(label->getHtml());
				auto o_fill = createFillStyle(builder, label->getBackground());

				fb::Rect rect;

				fb::CanvasLabelBuilder b_label(*builder);
				b_label.add_html(o_html);
				b_label.add_type(static_cast<fb::LabelType>(label->getStyleTypeInt()));
				b_label.add_valign(qt2fbVAlign(label->getVAlign()));
				b_label.add_fill_style(o_fill);
				b_label.add_rect(&rect);
				auto o_label = b_label.Finish();
			}

			fb::SlideBuilder b_slide(*builder);
			fb::CameraPosDirUp camera = Serializer::osg2fbCameraMatrix(slide->getCameraMatrix());
			b_slide.add_camera(&camera);
			b_slide.add_duration(slide->getDuration());
			b_slide.add_wait_for_click(slide->getStayOnNode());
			b_slide.add_transition_duration(slide->getTransitionDuration());
			auto o_slide = b_slide.Finish();
			v_slides.push_back(o_slide);

			osg::Matrixd m_camera_matrix;
			float m_duration;
			bool m_stay_on_node;
			float m_transition_duration;

			QImage m_thumbnail;
			bool m_thumbnail_dirty;
		}
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

	

	return flatbuffers::Offset<fb::NarrativeTable>();
}

void readLabelStyle(const fb::LabelStyle *buffer, LabelStyle *style)
{
	// fill style
	auto fs = buffer->fill_style();
	if (fs) {
		if (fs->color()) style->m_bg_color = Serializer::fb2qtColor(*fs->color());
	}

	// text style
	auto ts = buffer->text_style();
	if (ts) {
		if (ts->font_family()) style->m_font_family = ts->font_family()->str();
		if (ts->foreground()) style->m_fg_color = Serializer::fb2qtColor(*ts->foreground());
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
	createLabelStyle(flatbuffers::FlatBufferBuilder *builder, const LabelStyle *style)
{
	auto o_fill = createFillStyle(builder, style->m_bg_color);

	auto o_font = builder->CreateString(style->m_font_family);

	auto b_text = fb::TextStyleBuilder(*builder);
	b_text.add_font_family(o_font);
	auto fgcolor = Serializer::qt2fbColor(style->m_fg_color);
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
}

void NarrativeSerializer::readStyleTable(const fb::StyleTable *buffer, LabelStyleGroup * group)
{

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
