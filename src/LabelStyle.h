#ifndef LABELSTYLE_H
#define LABELSTYLE_H

#include <string>

#include <QColor>
#include <osg/Vec4>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QFont>
#include <QTextCursor>

#include "Group.h"
#include "LabelType.h"

class LabelStyle : public QObject, public osg::Node {
	Q_OBJECT

public:
	LabelStyle();
	//LabelStyle(Style base_style);
	LabelStyle(const LabelStyle& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	LabelStyle(LabelType type, const std::string font, int size, QColor fg, QColor bg,
		QFont::Weight we, bool ital, Qt::Alignment al, int m);
	virtual ~LabelStyle();

	void copy(const LabelStyle *other);

	META_Node(, LabelStyle);

	//void applyToLabel(NarrativeSlideLabel *label);
	//void applyToDocument(QTextDocument *doc);

	// serializer
	LabelType getType() const;
	void setType(LabelType s);
	int getTypeInt() const;
	void setTypeInt(int t);

	QColor backgroundColor() const;
	const osg::Vec4 &getBackgroundColor() const;
	void setBackgroundColor(const osg::Vec4 &color);

	int getAlign() const;
	void setAlign(int);
	void setVAlign(Qt::Alignment al);
	void setHAlign(Qt::Alignment al);
	int getMargin() const;
	void setMargin(int);

	QColor foregroundColor() const;
	const osg::Vec4 &getForegroundColor() const;
	void setForegroundColor(const osg::Vec4 &color);

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
	QColor m_bg_color;
	mutable osg::Vec4 m_bg_color_vec;
	// border width
	// border color
	// border style

	// text box properties
	Qt::Alignment m_align; // v align doesn't work on text documents, need to manually impl
	int m_margin;

	// text properties
	QColor m_fg_color;
	mutable osg::Vec4 m_fg_color_vec;
	std::string m_font_family;
	//std::string m_font_style; // font style is like a preset for weight+ital, we don't use this
	int m_weight;
	int m_point_size;
	bool m_ital;
	bool m_underline;
	bool m_strikethrough;
};

#endif /* LABELSTYLE_H */
