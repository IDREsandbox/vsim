#include "ModelDataTable.h"
#include <QDebug>
ModelDataTable::ModelDataTable()
{
}

ModelDataTable::ModelDataTable(const ModelDataTable & n, const osg::CopyOp & copyop)
{
}

void ModelDataTable::addMapping(osg::Node *node, ModelData *data)
{
	qDebug() << "inserting mapping" << node << data;
	m_table.insert(std::make_pair(node, data));
}
