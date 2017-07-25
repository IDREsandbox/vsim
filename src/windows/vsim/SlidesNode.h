#ifndef SLIDESNODE_H_
#define SLIDESNODE_H_

#include <osg/group>
#include <osg/image>
#include <QDebug>
#include <QImage>
#include <vector>

class VSCanvas;
namespace osgNewWidget { class Canvas; }

class SlidesNode : public osg::Group
{
public:
	enum SlidesNodeFlags { NONE, WITH_OVERLAY_CANVAS };

	SlidesNode(int flags = 0);
	SlidesNode(const SlidesNode& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	virtual ~SlidesNode();

	META_Node(, SlidesNode)

		//this can own the widget data. 3 vecs: x, y, text

	//std::vector<std::string> getTexts();
	//std::vector<std::string> getStyles();
	//std::vector<int> getWs();
	//std::vector<int> getHs();
	//std::vector<int> getXs();
	//std::vector<int> getYs();

	//void 

protected:
	std::vector<std::string> m_text;
	std::vector<std::string> m_style;
	std::vector<int> m_w;
	std::vector<int> m_h;
	std::vector<int> m_x;
	std::vector<int> m_y;
};

#endif /* SLIDESNODE_H_ */
