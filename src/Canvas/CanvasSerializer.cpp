#include "CanvasSerializer.h"

#include "Canvas/CanvasScene.h"
#include "Canvas/LabelStyleGroup.h"
#include "Canvas/LabelStyle.h"
#include "Core/TypesSerializer.h"

#include <QBuffer>

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

void readLineStyle(const fb::LineStyle *ls, CanvasItem *item)
{
	if (!ls) return;
	if (ls->color()) item->setBorderColor(TypesSerializer::fb2qtColor(*ls->color()));
	item->setBorderWidthPixels(ls->width());
	item->setHasBorder(ls->visible());
}

void readLineStyle(const fb::LineStyle *ls, FrameStyle *style)
{
	if (!ls) return;
	if (ls->color()) style->m_frame_color = TypesSerializer::fb2qtColor(*ls->color());
	style->m_frame_width = ls->width();
	style->m_has_frame = ls->visible();
}

flatbuffers::Offset<fb::LineStyle>
	createLineStyle(flatbuffers::FlatBufferBuilder *builder, const CanvasItem *item)
{
	auto fline_color = TypesSerializer::qt2fbColor(item->borderColor());
	fb::LineStyleBuilder b_line(*builder);
	b_line.add_color(&fline_color);
	b_line.add_visible(item->hasBorder());
	b_line.add_width(item->borderWidthPixels());
	auto o_line = b_line.Finish();
	return o_line;
}

flatbuffers::Offset<fb::LineStyle>
	createLineStyle(flatbuffers::FlatBufferBuilder *builder, const FrameStyle *style)
{
	auto fline_color = TypesSerializer::qt2fbColor(style->m_frame_color);
	fb::LineStyleBuilder b_line(*builder);
	b_line.add_color(&fline_color);
	b_line.add_visible(style->m_has_frame);
	b_line.add_width(style->m_frame_width);
	auto o_line = b_line.Finish();
	return o_line;
}


void readFillStyle(const fb::FillStyle *fs, CanvasItem *item)
{
	if (!fs) return;
	if (fs->color()) item->setBackground(TypesSerializer::fb2qtColor(*fs->color()));
}

void readFillStyle(const fb::FillStyle *fs, FrameStyle *style)
{
	if (!fs) return;
	if (fs->color()) style->m_bg_color = TypesSerializer::fb2qtColor(*fs->color());
}

QPixmap readPixmap(const VSim::FlatBuffers::CanvasImageData *d)
{
	if (!d) return QPixmap();
	QPixmap p;

	auto fb_ibuf = d->data();
	auto fb_ifmt = d->format();
	if (fb_ibuf && fb_ifmt) {
		bool ok = p.loadFromData(fb_ibuf->data(),
			fb_ibuf->size(), fb_ifmt->c_str());
	}

	return p;
}

flatbuffers::Offset<fb::CanvasImageData> createImageData(
	flatbuffers::FlatBufferBuilder *builder, QPixmap p)
{
	const char *fmt = "PNG";
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	p.save(&buffer, fmt); // writes pixmap into bytes in PNG format
	const uint8_t *ubuf = reinterpret_cast<const uint8_t *>(bytes.data());
	auto o_data = builder->CreateVector<unsigned char>(ubuf, bytes.length());

	auto o_fmt = builder->CreateString(fmt);

	fb::CanvasImageDataBuilder b_idata(*builder);
	b_idata.add_data(o_data);
	b_idata.add_format(o_fmt);
	auto o_idata = b_idata.Finish();

	return o_idata;
}

std::shared_ptr<CanvasItem> CanvasSerializer::readCanvasItem(
	const void *fb_item,
	const VSim::FlatBuffers::CanvasItem fb_type)
{
	if (!fb_item) return nullptr;

	auto item = std::shared_ptr<CanvasItem>(nullptr);
	const fb::FillStyle *fs = nullptr;
	const fb::LineStyle *ls = nullptr;
	const fb::Rect *fb_r = nullptr;

	if (fb_type == fb::CanvasItem_CanvasLabel) {
		auto fb_label = static_cast<const fb::CanvasLabel *>(fb_item);

		auto label = std::make_shared<CanvasLabel>();

		if (fb_label->html()) label->setHtml(fb_label->html()->str());
		label->setStyleType(static_cast<LabelType>(fb_label->type()));
		label->setVAlign(static_cast<Qt::Alignment>(fb_label->valign()));

		// read common
		item = label;
		fs = fb_label->fill_style();
		ls = fb_label->line_style();
		fb_r = fb_label->rect();
	}
	else if (fb_type == fb::CanvasItem_CanvasImage) {
		auto *fb_image = static_cast<const fb::CanvasImage *>(fb_item);

		auto image = std::make_shared<CanvasImage>();

		// read image
		image->setPixmap(readPixmap(fb_image->data()));

		// read common
		item = image;
		fs = fb_image->fill_style();
		ls = fb_image->line_style();
		fb_r = fb_image->rect();
	}

	// common item things
	if (item) {
		readFillStyle(fs, item.get());
		readLineStyle(ls, item.get());

		if (fb_r) {
			QRectF r(fb_r->x(), fb_r->y(), fb_r->w(), fb_r->h());
			item->setRect(r);
		}
	}

	return item;
}

bool CanvasSerializer::createCanvasItem(
	flatbuffers::FlatBufferBuilder * builder,
	const CanvasItem *item,
	flatbuffers::Offset<void> *out_offset,
	fb::CanvasItem *out_type)
{
	auto o_fill = createFillStyle(builder, item->background());
	auto o_line = createLineStyle(builder, item);

	QRectF qr = item->rect();
	fb::Rect fb_r(qr.x(), qr.y(), qr.width(), qr.height());

	const CanvasLabel *label = dynamic_cast<const CanvasLabel*>(item);
	if (label) {
		auto o_html = builder->CreateString(label->html());

		fb::CanvasLabelBuilder b_label(*builder);
		b_label.add_html(o_html);
		b_label.add_type(static_cast<fb::LabelType>(label->styleType()));
		b_label.add_valign(qt2fbVAlign(label->valign()));
		b_label.add_fill_style(o_fill);
		b_label.add_line_style(o_line);
		b_label.add_rect(&fb_r);
		auto o_label = b_label.Finish();

		*out_type = fb::CanvasItem::CanvasItem_CanvasLabel;
		*out_offset = o_label.Union();
		return true;
	}

	const CanvasImage *image = dynamic_cast<const CanvasImage*>(item);
	if (image) {
		// build image data
		auto o_idata = createImageData(builder, image->pixmap());

		fb::CanvasImageBuilder b_image(*builder);
		b_image.add_data(o_idata);
		b_image.add_fill_style(o_fill);
		b_image.add_line_style(o_line);
		b_image.add_rect(&fb_r);
		auto o_image = b_image.Finish();

		*out_type = fb::CanvasItem::CanvasItem_CanvasImage;
		*out_offset = o_image.Union();
		return true;
	}

	return false;
}

void CanvasSerializer::readCanvasItems(
	const flatbuffers::Vector<flatbuffers::Offset<void>> *items,
	const flatbuffers::Vector<uint8_t>* types,
	CanvasScene * scene)
{
	if (!items) return;
	for (size_t i = 0; i < items->size(); i++) {
		SharedItem item = readCanvasItem((*items)[i],
			static_cast<fb::CanvasItem>((*types)[i]));
		scene->addItem(item);
	};
}

void CanvasSerializer::readCanvas(const VSim::FlatBuffers::Canvas * buffer, CanvasScene *scene)
{
	readCanvasItems(buffer->items(), buffer->items_type(), scene);
}

flatbuffers::Offset<VSim::FlatBuffers::Canvas> CanvasSerializer::createCanvas(
	flatbuffers::FlatBufferBuilder * builder, const CanvasScene * scene)
{
	// build canvas items
	std::vector<uint8_t> v_item_types;
	std::vector<flatbuffers::Offset<void>> v_items;

	auto items = scene->itemList();
	for (std::shared_ptr<CanvasItem> item : items) {
		flatbuffers::Offset<void> fb_item;
		fb::CanvasItem fb_type;
		bool ok = createCanvasItem(builder, item.get(), &fb_item, &fb_type);
		if (!ok) continue;
		v_item_types.push_back(fb_type);
		v_items.push_back(fb_item);
	} // end items
	auto o_item_types = builder->CreateVector(v_item_types);
	auto o_items = builder->CreateVector(v_items);


	// build canvas
	fb::CanvasBuilder b_canvas(*builder);
	b_canvas.add_items(o_items);
	b_canvas.add_items_type(o_item_types);
	auto o_canvas = b_canvas.Finish();

	return o_canvas;
}

void CanvasSerializer::readLabelStyle(
	const fb::LabelStyle *buffer, LabelStyle *style)
{
	if (!buffer) return;

	// fill & line
	auto *fs = style->frameStyle();
	readFillStyle(buffer->fill_style(), fs);
	readLineStyle(buffer->line_style(), fs);

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
	CanvasSerializer::createLabelStyle(
		flatbuffers::FlatBufferBuilder *builder, const LabelStyle *style)
{
	// fill & line
	auto o_fill = createFillStyle(builder, style->cFrameStyle().m_bg_color);
	auto o_line = createLineStyle(builder, &style->cFrameStyle());

	// text
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

	// builder
	fb::LabelStyleBuilder b_style(*builder);
	b_style.add_fill_style(o_fill);
	b_style.add_line_style(o_line);
	b_style.add_text_style(o_text);
	b_style.add_type(static_cast<fb::LabelType>(style->getType()));
	auto o_style = b_style.Finish();

	return o_style;
}

void CanvasSerializer::readStyleTable(const fb::StyleTable *buffer,
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
	auto image = buffer->image();
	if (image) {
		auto *fs = group->getImageStyle();
		readLineStyle(image->line_style(), fs);
	}
}

flatbuffers::Offset<fb::StyleTable> CanvasSerializer::createStyleTable(
		flatbuffers::FlatBufferBuilder * builder, const LabelStyleGroup * group)
{
	LabelStyle *body = group->getStyle(LabelType::BODY);

	auto o_header1 = createLabelStyle(builder, group->getStyle(LabelType::HEADER1));
	auto o_header2 = createLabelStyle(builder, group->getStyle(LabelType::HEADER2));
	auto o_label = createLabelStyle(builder, group->getStyle(LabelType::LABEL));
	auto o_body = createLabelStyle(builder, group->getStyle(LabelType::BODY));
	
	auto o_line = createLineStyle(builder, group->getImageStyle());
	fb::ImageStyleBuilder b_image(*builder);
	b_image.add_line_style(o_line);
	auto o_image = b_image.Finish();

	fb::StyleTableBuilder b_styles(*builder);
	b_styles.add_header1(o_header1);
	b_styles.add_header2(o_header2);
	b_styles.add_label(o_label);
	b_styles.add_body(o_body);
	b_styles.add_image(o_image);
	return b_styles.Finish();
}
