#include "LabelStyle.h"
#include <QTextFrame>
#include <QTextCursor>
#include "Util.h"

LabelStyle::LabelStyle()
{
	m_base_document = new QTextDocument(this);
	m_base_document->setHtml("a");
}

//LabelStyle::LabelStyle(Style base_style)
//{
//	m_base_style = base_style;
//
//	switch (base_style) {
//	case NONE:
//		break;
//	case HEADER1:
//		break;
//	case HEADER2:
//		break;
//	case BODY:
//		break;
//	case LABEL:
//		break;
//	}
//}

LabelStyle::LabelStyle(const LabelStyle& n, const osg::CopyOp& copyop)
{
}

LabelStyle::LabelStyle(const std::string f, int s, int r, int g, int b, float o, bool sh, int rbg,
	int bbg, int gbg, float obg, int w, int h, QFont::Weight we, bool ital, Qt::Alignment al, int m)
	: LabelStyle()
{
	//font(f), size(s), red(r), green(g), blue(b), opacity(o),
	//	shadow(sh), red_bg(rbg), green_bg(gbg), blue_bg(bbg), opacity_bg(obg), width(w), height(h),
	//	weight(we), align(al), margin(m)

	m_width = w;
	m_height = h;
	m_bg_color = QColor(rbg, bbg, gbg, obg);

	// make a dummy document
	m_base_document = new QTextDocument(this);
	m_base_document->setHtml("a");

	// frame format
	m_frame_format = m_base_document->rootFrame()->frameFormat();
	m_frame_format.setMargin(m);
	m_frame_format.setBackground(QBrush(QColor(0, 0, 0, 0)));
	//m_frame_format.setBackground(QBrush(m_bg_color));

	// char format
	m_cursor = QTextCursor(m_base_document);
	m_cursor.select(QTextCursor::Document);
	m_char_format = m_cursor.charFormat();
	m_char_format.setFontFamily(QString::fromStdString(f));
	m_char_format.setForeground(QBrush(QColor(r, g, b, o)));
	m_char_format.setFontWeight(we);
	m_char_format.setFontItalic(ital);
	m_char_format.setFontPointSize(s);
	//m_char_format.setVerticalAlignment(QTextCharFormat::AlignMiddle);

	// block format
	m_block_format = m_cursor.blockFormat();
	m_block_format.setAlignment(al);
	// TODO: vertical alignment

	// apply formats
	m_cursor.setCharFormat(m_char_format);
	m_cursor.setBlockFormat(m_block_format);
	m_base_document->rootFrame()->setFrameFormat(m_frame_format);
}

LabelStyle::~LabelStyle()
{

}

QColor LabelStyle::backgroundColor() const
{
	return m_bg_color;
}

int LabelStyle::getBaseStyle() const
{
	return 0;
}

void LabelStyle::setBaseStyle(int style)
{
	m_style = (LabelStyle::Style)style;
}

const std::string &LabelStyle::getHtml() const
{
	m_html = m_base_document->toHtml().toStdString();
	return m_html;
}

void LabelStyle::setHtml(const std::string &html)
{
	m_base_document->setHtml(QString::fromStdString(html));
}

const osg::Vec4 &LabelStyle::getBackgroundColor() const
{
	m_bg_color_vec = Util::colorToVec(m_bg_color);
	return m_bg_color_vec;
}

void LabelStyle::setBackgroundColor(const osg::Vec4 &color)
{
	m_bg_color = Util::vecToColor(color);
}

//
//LabelStyle& LabelStyle::operator=(const LabelStyle& other)
//{
//	if (this != &other)
//	{
//		font = other.font;
//		size = other.size;
//		red = other.red;
//		green = other.green;
//		blue = other.blue;
//		opacity = other.opacity;
//		shadow = other.shadow;
//		red_bg = other.red_bg;
//		blue_bg = other.blue_bg;
//		green_bg = other.green_bg;
//		opacity_bg = other.opacity_bg;
//		width = other.width;
//		height = other.height;
//		weight = other.weight;
//		align = other.align;
//		margin = other.margin;
//	}
//
//	return *this;
//}
//
//void LabelStyle::applyToLabel(NarrativeSlideLabel * label)
//{
//}
//
//void LabelStyle::applyToDocument(QTextDocument * doc)
//{
//}
//
//
//const std::string& LabelStyle::getFont() const
//{
//	return font;
//}
//void LabelStyle::setFont(const std::string& f)
//{
//	font = f;
//}
//int LabelStyle::getSize() const
//{
//	return size;
//}
//void LabelStyle::setSize(int s)
//{
//	size = s;
//}
//int LabelStyle::getRed() const
//{
//	return red;
//}
//void LabelStyle::setRed(int r)
//{
//	red = r;
//}
//int LabelStyle::getBlue() const
//{
//	return blue;
//}
//void LabelStyle::setBlue(int b)
//{
//	blue = b;
//}
//int LabelStyle::getGreen() const
//{
//	return green;
//}
//void LabelStyle::setGreen(int g)
//{
//	green = g;
//}
//float LabelStyle::getOpacity() const
//{
//	return opacity;
//}
//void LabelStyle::setOpacity(float o)
//{
//	opacity = o;
//}
//bool LabelStyle::getShadow() const
//{
//	return shadow;
//}
//void LabelStyle::setShadow(bool s)
//{
//	shadow = s;
//}
//
//int LabelStyle::getRed_BG() const
//{
//	return red_bg;
//}
//void LabelStyle::setRed_BG(int rbg)
//{
//	red_bg = rbg;
//}
//int LabelStyle::getBlue_BG() const
//{
//	return blue_bg;
//}
//void LabelStyle::setBlue_BG(int bbg)
//{
//	blue_bg = bbg;
//}
//int LabelStyle::getGreen_BG() const
//{
//	return green_bg;
//}
//void LabelStyle::setGreen_BG(int gbg)
//{
//	green_bg = gbg;
//}
//float LabelStyle::getOpacity_BG() const
//{
//	return opacity_bg;
//}
//void LabelStyle::setOpacity_BG(float obg)
//{
//	opacity_bg = obg;
//}
//int LabelStyle::getWidth() const
//{
//	return width;
//}
//void LabelStyle::setWidth(int w)
//{
//	width = w;
//}
//int LabelStyle::getHeight() const
//{
//	return height;
//}
//void LabelStyle::setHeight(int h)
//{
//	height = h;
//}
//
//const std::string& LabelStyle::getWeight() const
//{
//	return weight;
//}
//void LabelStyle::setWeight(const std::string& Weight)
//{
//	weight = Weight;
//}
//const std::string& LabelStyle::getAlign() const
//{
//	return align;
//}
//void LabelStyle::setAlign(const std::string& Align)
//{
//	align = Align;
//}
//
//int LabelStyle::getMargin() const
//{
//	return margin;
//}
//
//void LabelStyle::setMargin(int m)
//{
//	margin = m;
//}