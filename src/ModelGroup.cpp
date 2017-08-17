#include "ModelGroup.h"
#include <QDebug>

ModelGroup::ModelGroup()
	: m_year(0)
{
}

ModelGroup::ModelGroup(const ModelGroup & n, const osg::CopyOp & copyop)
	: m_year(n.m_year)
{
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
		ModelNode *model = dynamic_cast<ModelNode*>(node);
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
		ModelNode *info = dynamic_cast<ModelNode*>(node.getChild(i));
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
		ModelNode *info = dynamic_cast<ModelNode*>(node.getChild(i));
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
