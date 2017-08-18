#ifndef MODELDATATABLE_H
#define MODELDATATABLE_H

#include <QObject>
#include <osg/Observer>
#include "ModelData.h"

class ModelDataTable : public QObject, public osg::Node {
	Q_OBJECT
public:
	ModelDataTable();
	ModelDataTable(const ModelDataTable& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	META_Node(, ModelDataTable);

	void addMapping(osg::Node *node, ModelData *data);

	ModelData *getData(osg::Node *node);

public:
	std::set<osg::Observer> m_observers;
	std::map<osg::Node*, osg::ref_ptr<ModelData>> m_table;
};

#endif