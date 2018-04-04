#ifndef LABELSTYLEGROUP
#define LABELSTYLEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include <memory>
#include "Group.h"
#include "LabelType.h"


class LabelStyle;

class LabelStyleGroup : public Group {
	Q_OBJECT;
public:
	LabelStyleGroup();
	~LabelStyleGroup();

	LabelStyle *getStyle(LabelType style) const;

private:
	std::unique_ptr<LabelStyle> m_h1;
	std::unique_ptr<LabelStyle> m_h2;
	std::unique_ptr<LabelStyle> m_bod;
	std::unique_ptr<LabelStyle> m_lab;
	//osg::ref_ptr<LabelStyle> m_img;
};

#endif // LABELSTYLEGROUP_H