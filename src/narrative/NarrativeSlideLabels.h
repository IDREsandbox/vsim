#ifndef NARRATIVESLIDELABELS_H
#define NARRATIVESLIDELABELS_H

#include <osg/Group>
#include <osg/Image>
#include <QDebug>

class NarrativeSlideLabels : public osg::Node
{
public:
	NarrativeSlideLabels(); 
	NarrativeSlideLabels(const NarrativeSlideLabels& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	
	//TODO: add op to copy from legacy narr
	
	virtual ~NarrativeSlideLabels();

	META_Node(, NarrativeSlideLabels);

	float getrX() const;
	void setrX(float x);
	float getrY() const;
	void setrY(float y);
	float getrW() const;
	void setrW(float w);
	float getrH() const;
	void setrH(float h);

	const std::string& getText() const;
	void setText(const std::string& text);
	const std::string& getStyle() const;
	void setStyle(const std::string& style);

signals:


public: // COMMANDS



public:
	float m_rX;
	float m_rY;
	float m_rW;
	float m_rH;

	std::string m_text;
	std::string m_style;
};

#endif /* NARRATIVESLIDELABELS_H */
