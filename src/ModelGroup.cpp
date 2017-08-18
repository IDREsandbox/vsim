#include "ModelGroup.h"
#include <QDebug>

ModelGroup::ModelGroup()
	: m_year(0)
{
	m_data_table = new ModelDataTable();
	qDebug() << "new model table" << m_data_table.get();
}

ModelGroup::ModelGroup(const ModelGroup & n, const osg::CopyOp & copyop)
	: m_year(n.m_year)
{
	m_data_table = new ModelDataTable();
	qDebug() << "Model Group copy constructor?";
}

void ModelGroup::merge(ModelGroup *other)
{
	for (uint i = 0; i < other->getNumChildren(); i++) {
		addChild(other->getChild(i));
	}
	// also merge the mappings
	for (auto &kv : other->dataTable()->m_table) {
		m_data_table->addMapping(kv.first, kv.second.get());
	}
}

ModelDataTable * ModelGroup::dataTable() const
{
	qDebug() << "get data table" << m_data_table.get();
	return m_data_table.get();
}

const ModelDataTable * ModelGroup::getDataTable() const
{
	qDebug() << "get data table const" << m_data_table.get();
	return m_data_table.get();
}

void ModelGroup::setDataTable(ModelDataTable *table)
{
	if (!table) m_data_table = new ModelDataTable;
	else m_data_table = table;
	qDebug() << "set data table" << m_data_table.get();
}

int ModelGroup::getYear() const
{
	return m_year;
}

void ModelGroup::setYear(int year)
{
	m_year = year;
	for (uint i = 0; i < getNumChildren(); i++) {
		osg::Node *node = getChild(i);
		ModelData *model = dynamic_cast<ModelData*>(node);
		if (!model) {
			qWarning() << "Non-model in ModelGroup at index" << i;
			return;
		}

		if (year == 0) {
			model->setNodeMask(~0);
		}
		else if (year >= model->getYearBegin() && year <= model->getYearEnd()) {
			model->setNodeMask(~0);
		}
		else {
			model->setNodeMask(0);
		}
	}
	emit sYearChange(year);
}

std::set<int> ModelGroup::getKeyYears()
{
	std::set<int> years;
	TimeGetVisitor v(&years);
	accept(v);

	if (years.empty()) return years;

	return years;
}

TimeGetVisitor::TimeGetVisitor(std::set<int>* out)
	: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
	m_out(out) {}
void TimeGetVisitor::apply(osg::Group &node)
{
	// look for ModelNodes
	// add the years to the set
	for (uint i = 0; i < node.getNumChildren(); i++) {
		ModelData *info = dynamic_cast<ModelData*>(node.getChild(i));
		if (!info) continue;

		m_out->insert(info->getYearEnd() + 1);
		m_out->insert(info->getYearBegin());
	}
}

TimeMaskVisitor::TimeMaskVisitor(int year)
	: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
	m_year(year) {}
void TimeMaskVisitor::apply(osg::Group &node)
{
	// look for ModelNodes
	// mask the owning groups if the year is out of range
	for (uint i = 0; i < node.getNumChildren(); i++) {
		ModelData *info = dynamic_cast<ModelData*>(node.getChild(i));
		if (!info) continue;

		if (m_year == 0) {
			node.setNodeMask(~0);
		}
		else if (m_year >= info->getYearBegin() && m_year <= info->getYearEnd()) {
			node.setNodeMask(~0);
		}
		else {
			node.setNodeMask(0);
		}
	}
}
