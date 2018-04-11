#ifndef LABELSTYLEGROUP_H
#define LABELSTYLEGROUP_H

#include <QObject>
#include <QUndoCommand>
#include <osg/Group>
#include <memory>
#include "GroupTemplate.h"
#include "LabelType.h"


class LabelStyle;

class LabelStyleGroup : public TGroup<LabelStyle> {
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
};

#endif // LABELSTYLEGROUP_H