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

class LabelStyle : public QObject, public osg::Node {
	Q_OBJECT

public:
	enum Style {
		NONE,
		HEADER1,
		HEADER2,
		BODY,
		LABEL
	};
	static constexpr const char *StyleNames[] = {
		"None",
		"Header 1",
		"Header 2",
		"Body",
		"Label"
	};

	LabelStyle();
	//LabelStyle(Style base_style);
	LabelStyle(const LabelStyle& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	LabelStyle(const std::string f, int s, int r, int g, int b, float o, bool sh, int rbg, 
		int bbg, int gbg, float obg, int w, int h, QFont::Weight we, bool ital, Qt::Alignment al, int m);
	virtual ~LabelStyle();

	//LabelStyle& operator=(const LabelStyle& other);

	META_Node(, LabelStyle);

	//void applyToLabel(NarrativeSlideLabel *label);
	//void applyToDocument(QTextDocument *doc);

	QColor backgroundColor() const;

	// serialized
	int getBaseStyle() const;
	void setBaseStyle(int style);
	const std::string &getHtml() const;
	void setHtml(const std::string &html);
	const osg::Vec4 &getBackgroundColor() const;
	void setBackgroundColor(const osg::Vec4 &color);

	//const std::string& getFont() const;
	//void setFont(const std::string& f);
	//int getSize() const;
	//void setSize(int s);
	//int getRed() const;
	//void setRed(int r);
	//int getBlue() const;
	//void setBlue(int b);
	//int getGreen() const;
	//void setGreen(int g);
	//float getOpacity() const;
	//void setOpacity(float o);
	//bool getShadow() const;
	//void setShadow(bool s);

	//int getRed_BG() const;
	//void setRed_BG(int rbg);
	//int getBlue_BG() const;
	//void setBlue_BG(int bbg);
	//int getGreen_BG() const;
	//void setGreen_BG(int gbg);
	//float getOpacity_BG() const;
	//void setOpacity_BG(float obg);
	//int getWidth() const;
	//void setWidth(int w);
	//int getHeight() const;
	//void setHeight(int h);
	//int getMargin() const;
	//void setMargin(int m);

	//const std::string& getWeight() const;
	//void setWeight(const std::string& Weight);
	//const std::string& getAlign() const;
	//void setAlign(const std::string& Align);

//signals:
//	void sTitleChanged(const std::string&);
//	void sAuthorChanged(const std::string&);
//	void sDescriptionChanged(const std::string&);

//public: // COMMANDS
//
//	class SetTitleCommand : public ModifyCommand<LabelStyle, const std::string&> {
//	public:
//		SetTitleCommand(LabelStyle *nar, const std::string &title, QUndoCommand *parent = nullptr)
//			: ModifyCommand(&getTitle, &setTitle, title, nar, parent) {}
//	};
//	class SetAuthorCommand : public ModifyCommand<LabelStyle, const std::string&> {
//	public:
//		SetAuthorCommand(LabelStyle *nar, const std::string &author, QUndoCommand *parent = nullptr)
//			: ModifyCommand(&getAuthor, &setAuthor, author, nar, parent) {}
//	};
//	class SetDescriptionCommand : public ModifyCommand<LabelStyle, const std::string&> {
//	public:
//		SetDescriptionCommand(LabelStyle *nar, const std::string &desc, QUndoCommand *parent = nullptr)
//			: ModifyCommand(&getDescription, &setDescription, desc, nar, parent) {}
//	};

public:
	Style m_base_style;
	Style m_style;
	QTextCharFormat m_char_format;
	QTextBlockFormat m_block_format;
	QTextFrameFormat m_frame_format;
	QTextDocument *m_base_document;
	QTextCursor m_cursor;

	mutable std::string m_html;

	int m_width;
	int m_height;

	QColor m_bg_color;
	mutable osg::Vec4 m_bg_color_vec;
};

#endif /* LABELSTYLE_H */
