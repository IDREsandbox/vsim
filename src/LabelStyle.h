#pragma once
#ifndef LABELSTYLE_H
#define LABELSTYLE_H

#include <string>
#include <osg/Node>
#include <QObject>
#include "narrative/NarrativeSlide.h"
#include "deprecated/narrative/Narrative.h"
#include "Group.h"
#include "LabelStyle.h"

class LabelStyle : public Group {
	Q_OBJECT

public:
	LabelStyle();
	LabelStyle(const LabelStyle& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	LabelStyle(const std::string f, int s, int r, int g, int b, float o, bool sh, int rbg, 
		int bbg, int gbg, float obg, int w, int h, const std::string we, const std::string al);
	virtual ~LabelStyle();

	LabelStyle& operator=(const LabelStyle& other);

	META_Node(, LabelStyle)

	const std::string& getFont() const;
	void setFont(const std::string& f);
	int getSize() const;
	void setSize(int s);
	int getRed() const;
	void setRed(int r);
	int getBlue() const;
	void setBlue(int b);
	int getGreen() const;
	void setGreen(int g);
	float getOpacity() const;
	void setOpacity(float o);
	bool getShadow() const;
	void setShadow(bool s);

	int getRed_BG() const;
	void setRed_BG(int rbg);
	int getBlue_BG() const;
	void setBlue_BG(int bbg);
	int getGreen_BG() const;
	void setGreen_BG(int gbg);
	float getOpacity_BG() const;
	void setOpacity_BG(float obg);
	int getWidth() const;
	void setWidth(int w);
	int getHeight() const;
	void setHeight(int h);

	const std::string& getWeight() const;
	void setWeight(const std::string& Weight);
	const std::string& getAlign() const;
	void setAlign(const std::string& Align);

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


private:
	std::string font;
	int	size;
	int red;
	int blue;
	int green;
	float opacity;
	bool shadow;

	int red_bg;
	int blue_bg;
	int green_bg;
	float opacity_bg;
	int width;
	int height;

	std::string weight;
	std::string align;
};

#endif /* LABELSTYLE_H */
