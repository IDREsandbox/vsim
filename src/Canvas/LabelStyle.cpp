#include "Canvas/LabelStyle.h"
#include <QTextFrame>
#include <QTextCursor>
#include <QDebug>
#include <algorithm>

static QString stringRGB(QColor c) {
	return QString().sprintf("rgb(%d, %d, %d)",
		c.red(), c.green(), c.blue()
	);
}
static QString stringRGBA(QColor c) {
	return QString().sprintf("rgba(%d, %d, %d, %d)",
		c.red(), c.green(), c.blue(), c.alpha()
	);
}

FrameStyle::FrameStyle()
	: m_bg_color(QColor(0, 0, 0)),
	m_frame_color(QColor(0, 0, 0)),
	m_frame_width(0),
	m_has_frame(false)
{
}

FrameStyle::FrameStyle(QColor background, QColor frame_color, int frame_width, bool has_frame)
	: m_bg_color(background),
	m_frame_color(frame_color),
	m_frame_width(frame_width),
	m_has_frame(has_frame)
{
}

FrameStyle::~FrameStyle()
{
}

void FrameStyle::applyToWidget(QWidget * w) const
{
	w->setStyleSheet(styleText());
}

QString FrameStyle::styleText() const
{
	QString str =
		"background:" + stringRGBA(m_bg_color) + ";"
		"border:" + "1px solid " + stringRGB(m_frame_color) + ";";
	return str;
}

LabelStyle::LabelStyle()
	: m_type(NONE),
	m_align(Qt::AlignTop | Qt::AlignLeft),
	m_fg_color(0, 0, 0),
	m_point_size(12),
	m_weight(QFont::Weight::Normal),
	m_ital(false),
	m_underline(false)
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
	m_frame.m_bg_color = bg;

	// text
	m_font_family = font;
	m_point_size = size;
	m_fg_color = fg;
	m_weight = we;
	m_ital = ital;
}

LabelStyle::~LabelStyle()
{
}

void LabelStyle::copy(const LabelStyle *other)
{
	*this = *other;
}

void LabelStyle::applyToWidget(QWidget * widget, bool font_size)
{
	// doesn't work for QPushButton and other native? stuff
	//widget->setBackgroundRole(QPalette::Background);
	//widget->setAutoFillBackground(true);
	//QPalette p = widget->palette();
	//p.setBrush(widget->backgroundRole(), m_bg_color);
	//p.setBrush(widget->foregroundRole(), m_fg_color);
	//p.setBrush(QPalette::ButtonText, m_fg_color);
	//widget->setPalette(p);

	// color stylesheet
	QString ss = "color:" + stringRGB(m_fg_color) + ";";
	ss += m_frame.styleText();
	widget->setStyleSheet(ss);

	QFont f = widget->font();
	f.setFamily(m_font_family.c_str());
	f.setWeight(m_weight);
	if (font_size) f.setPointSize(m_point_size);
	f.setItalic(m_ital);
	f.setUnderline(m_underline);
	widget->setFont(f);

	// label->setAlignment(m_align); // QLabel only
	// widget->setMargin(m_margin); // QLabel only
}

void LabelStyle::applyToDocument(QTextDocument * doc)
{
	QTextFrameFormat tff;
	tff.setMargin(getMargin());
	tff.setBackground(QBrush(QColor(0, 0, 0, 0)));

	QTextBlockFormat tbf;
	tbf.setAlignment(m_align & Qt::AlignHorizontal_Mask);

	QTextCharFormat tcf;
	QString ff = QString::fromStdString(getFontFamily());
	if (!ff.isEmpty()) tcf.setFontFamily(ff);
	tcf.setFontWeight(getWeight());
	tcf.setForeground(foregroundColor());
	tcf.setFontPointSize(getPointSize());
	tcf.setFontItalic(getItalicized());
	tcf.setFontUnderline(getUnderline());

	// assign formats to document
	QTextCursor cursor = QTextCursor(doc);
	cursor.select(QTextCursor::Document);
	cursor.mergeCharFormat(tcf);
	cursor.mergeBlockCharFormat(tcf);
	cursor.mergeBlockFormat(tbf);
	QTextFrameFormat tff_merged = doc->rootFrame()->frameFormat();
	tff_merged.merge(tff);
	doc->rootFrame()->setFrameFormat(tff_merged);
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

FrameStyle *LabelStyle::frameStyle()
{
	return &m_frame;
}

const FrameStyle & LabelStyle::cFrameStyle() const
{
	return m_frame;
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

Qt::Alignment LabelStyle::valign() const
{
	return m_align & Qt::AlignVertical_Mask;
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

void LabelStyle::setForegroundColor(const QColor & c)
{
	m_fg_color = c;
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
	m_weight = std::clamp(w, 0, 99);
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
