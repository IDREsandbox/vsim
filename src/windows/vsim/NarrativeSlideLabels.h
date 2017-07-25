#ifndef NARRATIVESLIDELABELS_H
#define NARRATIVESLIDELABELS_H

#include <osg/group>
#include <osg/image>
#include <QDebug>

class NarrativeSlideLabels : public osg::Node
{
public:
	NarrativeSlideLabels(); 
	NarrativeSlideLabels(const NarrativeSlideLabels& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	
	//TODO: add op to copy from legacy narr
	
	virtual ~NarrativeSlideLabels();

	META_Node(, NarrativeSlideLabels);

	int getX() const;
	void setX(int x);
	int getY() const;
	void setY(int y);
	int getW() const;
	void setW(int w);
	int getH() const;
	void setH(int h);

	int getParW() const;
	void setParW(int pw);
	int getParH() const;
	void setParH(int ph);
	float getRatW() const;
	void setRatW(float rw);
	float getRatH() const;
	void setRatH(float rh);

	const std::string& getText() const;
	void setText(const std::string& text);
	const std::string& getStyle() const;
	void setStyle(const std::string& style);

public:
	int m_x;
	int m_y;
	int m_w;
	int m_h;

	int m_parH;
	int m_parW;
	float m_ratW;
	float m_ratH;

	std::string m_text;
	std::string m_style;
};

#endif /* NARRATIVESLIDELABELS_H */
