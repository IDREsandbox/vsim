#include "LabelStyle.h"
#include <QTextFrame>
#include <QTextCursor>
#include "Util.h"

LabelStyle::LabelStyle()
	: m_type(NONE),
	m_bg_color(255, 255, 255),
	m_align(Qt::AlignTop | Qt::AlignLeft),
	m_fg_color(0, 0, 0),
	m_point_size(12),
	m_weight(QFont::Weight::Normal),
	m_ital(false),
	m_underline(false),
	m_strikethrough(false)
{
}

LabelStyle::LabelStyle(const LabelStyle& n, const osg::CopyOp& copyop)
{
}

LabelStyle::LabelStyle(LabelType type, std::string font, int size, QColor fg, QColor bg,
	QFont::Weight we, bool ital, Qt::Alignment al, int m)
	: LabelStyle()
{
	m_type = type;

	// rect
	m_align = al;
	m_margin = m;
	m_bg_color = bg;

	// text
	m_font_family = font;
	m_point_size = size;
	m_fg_color = fg;
	m_weight = we;
	m_ital = ital;


	//font(f), size(s), red(r), green(g), blue(b), opacity(o),
	//	shadow(sh), red_bg(rbg), green_bg(gbg), blue_bg(bbg), opacity_bg(obg), width(w), height(h),
	//	weight(we), align(al), margin(m)

	//m_width = w;
	//m_height = h;
	//m_bg_color = QColor(rbg, bbg, gbg, obg);

	//// make a dummy document
	//m_base_document = new QTextDocument(this);
	//m_base_document->setHtml("a");

	//// frame format
	//m_frame_format = m_base_document->rootFrame()->frameFormat();
	//m_frame_format.setMargin(m);
	//m_frame_format.setBackground(QBrush(QColor(0, 0, 0, 0)));
	////m_frame_format.setBackground(QBrush(m_bg_color));

	//// char format
	//m_cursor = QTextCursor(m_base_document);
	//m_cursor.select(QTextCursor::Document);
	//m_char_format = m_cursor.charFormat();
	//m_char_format.setFontFamily(QString::fromStdString(f));
	//m_char_format.setForeground(QBrush(QColor(r, g, b, o)));
	//m_char_format.setFontWeight(we);
	//m_char_format.setFontItalic(ital);
	//m_char_format.setFontPointSize(s);
	////m_char_format.setVerticalAlignment(QTextCharFormat::AlignMiddle);

	//// block format
	//m_block_format = m_cursor.blockFormat();
	//m_block_format.setAlignment(al);
	//// TODO: vertical alignment

	//// apply formats
	//m_cursor.setCharFormat(m_char_format);
	//m_cursor.setBlockFormat(m_block_format);
	//m_base_document->rootFrame()->setFrameFormat(m_frame_format);
}

LabelStyle::~LabelStyle()
{

}

void LabelStyle::copy(const LabelStyle *other)
{
	m_type = other->m_type;
	m_bg_color = other->m_bg_color;
	m_align = other->m_align;
	m_margin = other->m_margin;
	m_fg_color = other->m_fg_color;
	m_font_family = other->m_font_family;
	m_weight = other->m_weight;
	m_point_size = other->m_point_size;
	m_ital = other->m_ital;
	m_underline = other->m_underline;
	m_strikethrough = other->m_strikethrough;
}

QColor LabelStyle::backgroundColor() const
{
	return m_bg_color;
}
LabelType LabelStyle::getType() const
{
	return m_type;
}
void LabelStyle::setType(LabelType s)
{
	m_type = s;
}
int LabelStyle::getTypeInt() const
{
	return m_type;
}
void LabelStyle::setTypeInt(int t)
{
	m_type = static_cast<LabelType>(t);
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

int LabelStyle::getAlign() const
{
	return m_align;
}

void LabelStyle::setAlign(int a)
{
	m_align = static_cast<Qt::Alignment>(a);
}

void LabelStyle::setVAlign(Qt::Alignment al)
{
	// keep current horizontal, add on vertical
	m_align = (m_align & Qt::AlignHorizontal_Mask) | (al & Qt::AlignVertical_Mask);
}

void LabelStyle::setHAlign(Qt::Alignment al)
{
	// keep current vertical, add on horizontal
	m_align = (m_align & Qt::AlignVertical_Mask) | (al & Qt::AlignHorizontal_Mask);
}

int LabelStyle::getMargin() const
{
	return m_margin;
}

void LabelStyle::setMargin(int m)
{
	m_margin = std::max(0, m);
}

QColor LabelStyle::foregroundColor() const
{
	return m_fg_color;
}

const osg::Vec4 & LabelStyle::getForegroundColor() const
{
	m_fg_color_vec = Util::colorToVec(m_fg_color);
	return m_fg_color_vec;
}

void LabelStyle::setForegroundColor(const osg::Vec4 & color)
{
	m_fg_color = Util::vecToColor(color);
}

const std::string & LabelStyle::getFontFamily() const
{
	return m_font_family;
}

void LabelStyle::setFontFamily(const std::string & f)
{
	m_font_family = f;
}

int LabelStyle::getPointSize() const
{
	return m_point_size;
}

void LabelStyle::setPointSize(int p)
{
	m_point_size = std::max(p, 1);
}

int LabelStyle::getWeight() const
{
	return m_weight;
}

void LabelStyle::setWeight(int w)
{
	m_weight = Util::clamp(w, 0, 99);
}

bool LabelStyle::getItalicized() const
{
	return m_ital;
}

void LabelStyle::setItalicized(bool i)
{
	m_ital = i;
}

bool LabelStyle::getUnderline() const
{
	return m_underline;
}

void LabelStyle::setUnderline(bool u)
{
	m_underline = u;
}