#ifndef MODELGROUP_H
#define MODELGROUP_H

#include <QObject>
#include <osg/Group>

class ModelGroup : public QObject, public osg::Group {
	Q_OBJECT
public:
	ModelGroup() {}
	ModelGroup(const ModelGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	META_Node(, ModelGroup)

};

#endif