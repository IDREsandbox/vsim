#include <osgDB/ObjectWrapper>
#include <QDebug>
#include "ModelDataTable.h"

static bool checkDataTable(const ModelDataTable& table)
{
	qDebug() << "check data table" << table.m_table.size();
	return table.m_table.size() > 0;
}

static bool readDataTable(osgDB::InputStream& is, ModelDataTable& table)
{
	qDebug() << "read data table";
	unsigned int size = 0;
	is >> size >> is.BEGIN_BRACKET;
	for (unsigned int i = 0; i < size; i++)
	{
		qDebug() << "reading mapping";
		osg::Node* node = dynamic_cast<ModelDataTable*>(is.readObject());
		if (!node) {
			qDebug() << "ModelDataTable serializer - failed to read node" << i;
			return false;
		}

		ModelData* data = dynamic_cast<ModelData*>(is.readObject());
		if (!data) {
			qDebug() << "ModelDataTable serializer - failed to read data" << i;
			return false;
		}

		table.addMapping(node, data);
	}
	is >> is.END_BRACKET;
	return true;
}

static bool writeDataTable(osgDB::OutputStream& os, const ModelDataTable& table)
{
	qDebug() << "write data table";
	unsigned int size = table.m_table.size();
	os << size << os.BEGIN_BRACKET << std::endl;

	// std::map<osg::Node*, osg::ref_ptr<ModelData>>
	for (auto &kv : table.m_table) {
		qDebug() << "writing mapping";
		os << kv.first;
		os << kv.second.get(); 
	}

	os << os.END_BRACKET << std::endl;
	return true;
}

REGISTER_OBJECT_WRAPPER(
	ModelDataTable,
	new ::ModelDataTable,
	::ModelDataTable,
	"osg::Object osg::Node ::ModelDataTable")
{
	ADD_USER_SERIALIZER(DataTable);
	//ADD_LIST_SERIALIZER(List, std::vector<ModelDataMapping>); // this doesn't work because of object shenanigans
}
