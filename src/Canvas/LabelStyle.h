#ifndef LABELSTYLE_H
#define LABELSTYLE_H

#include <string>

#include <QColor>
#include <osg/Vec4>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QFont>
#include <QTextCursor>
#include <QLabel>

#include "Canvas/LabelType.h"

class FrameStyle {
public:
	FrameStyle();
	FrameStyle(QColor background, QColor frame_color, int frame_width, bool has_frame);
	~FrameStyle();

	// operator=(); // default

	void applyToWidget(QWidget *w) const;
	QString styleText() const;

public:
	QColor m_bg_color;
	QColor m_frame_color;
	int m_frame_width;
	bool m_has_frame;
};

class LabelStyle {
public:
	LabelStyle();
	LabelStyle(LabelType type, const std::string font, int size, QColor fg, QColor bg,
		QFont::Weight we, bool ital, Qt::Alignment al, int m);
	~LabelStyle();

	// LabelStyle::operator=(); // default
	void copy(const LabelStyle *other);

	void applyToWidget(QWidget *widget, bool font_size = true);
	void applyToDocument(QTextDocument *doc); // doesn't apply valign, or frame
	// implementation in CanvasControl
	//void applyToCanvasLabel(CanvasLabel *label);

	// serializer
	LabelType getType() const;
	void setType(LabelType s);
	int getTypeInt() const;
	void setTypeInt(int t);

	FrameStyle *frameStyle(); // FIXME: better style than raw pointer?
	const FrameStyle &cFrameStyle() const;

	//QColor backgroundColor() const;
	//const osg::Vec4 &getBackgroundColor() const;
	//void setBackgroundColor(const osg::Vec4 &color);

	int getAlign() const;
	void setAlign(int);
	void setVAlign(Qt::Alignment al);
	void setHAlign(Qt::Alignment al);
	Qt::Alignment valign() const;
	int getMargin() const;
	void setMargin(int);

	QColor foregroundColor() const;
	void setForegroundColor(const QColor &c);

	const std::string &getFontFamily() const;
	void setFontFamily(const std::string &f);
	int getPointSize() const;
	void setPointSize(int p);
	int getWeight() const;
	void setWeight(int);
	bool getItalicized() const;
	void setItalicized(bool i);
	bool getUnderline() const;
	void setUnderline(bool u);
	//bool getStrikethrough() const;
	//void setStrikethrough();

public:
	LabelType m_type;

	// box properties
	//int m_width;
	//int m_height;
	FrameStyle m_frame;

	// text box properties
	Qt::Alignment m_align; // v align doesn't work on text documents, need to manually impl
	int m_margin;

	// text properties
	QColor m_fg_color;
	std::string m_font_family;
	//std::string m_font_style; // font style is like a preset for weight+ital, we don't use this
	int m_weight;
	int m_point_size;
	bool m_ital;
	bool m_underline;
	//bool m_strikethrough;
};

#endif /* LABELSTYLE_H */
