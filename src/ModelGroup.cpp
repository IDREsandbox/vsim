#include "ModelGroup.h"
#include <QDebug>
#include <regex>
#include "Util.h"

ModelGroup::ModelGroup()
	: m_year(0)
{
	//qDebug() << "new model table" << m_data_table.get();
}

ModelGroup::ModelGroup(const ModelGroup & n, const osg::CopyOp & copyop)
	: m_year(n.m_year)
{
	qDebug() << "Model Group copy constructor?";
}

void ModelGroup::merge(ModelGroup *other)
{
	for (uint i = 0; i < other->getNumChildren(); i++) {
		addChild(other->getChild(i));
	}
}

int ModelGroup::getYear() const
{
	return m_year;
}

void ModelGroup::setYear(int year)
{
	m_year = year;

	// make a year traverser
	TimeMaskVisitor v(year);
	accept(v);

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

bool ModelGroup::addChild(osg::Node *child)
{
	bool ok = Group::addChild(child);
	if (!ok) return false;
	TimeInitVisitor v; // check for T: start end
	child->accept(v);
	return true;
}

bool ModelGroup::nodeTimeInName(const std::string & name, int * begin, int * end)
{
	std::regex r(".*T:.* (-?\\d+) (-?\\d+)");
	std::smatch match;
	if (std::regex_search(name, match, r)) {
		*begin = std::stoi(match[1]);
		*end = std::stoi(match[2]);
		return true;
	}
	return false;
}

TimeInitVisitor::TimeInitVisitor()
	: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
}
void TimeInitVisitor::apply(osg::Group &node)
{
	std::string name = node.getName();
	qDebug() << "applying to" << QString::fromStdString(name);

	int begin, end;
	bool match = ModelGroup::nodeTimeInName(node.getName(), &begin, &end);
	if (match) {
		if (begin != 0) {
			node.setUserValue("yearBegin", begin);
		}
		if (end != 0) {
			node.setUserValue("yearEnd", end);
		}
	}
	traverse(node);
}

TimeGetVisitor::TimeGetVisitor(std::set<int>* out)
	: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
	m_out(out) {}
void TimeGetVisitor::apply(osg::Group &group)
{
	// look for ModelNodes
	// add the years to the set
	for (uint i = 0; i < group.getNumChildren(); i++) {
		osg::Node *node = group.getChild(i);
		int begin, end;
		bool beginok, endok;
		beginok = node->getUserValue("yearBegin", begin);
		endok = node->getUserValue("yearEnd", end);

		//m_out->insert(info->getYearEnd() + 1);
		//m_out->insert(info->getYearBegin());
	}
}

TimeMaskVisitor::TimeMaskVisitor(int year)
	: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
	m_year(year) {}
void TimeMaskVisitor::apply(osg::Group &group)
{
	// look for ModelNodes
	// mask the owning groups if the year is out of range
	for (uint i = 0; i < group.getNumChildren(); i++) {
		osg::Node *node = group.getChild(i);
		int begin, end;
		bool beginok, endok;
		beginok = node->getUserValue("yearBegin", begin);
		endok = node->getUserValue("yearEnd", end);

		// see everything
		if (m_year == 0) {
			node->setNodeMask(~0);
		}
		// in-range
		// If begin isn't there, or begin is 0, or year >= begin, then ok
		else if ((beginok == false || begin == 0 || m_year >= begin) &&
			(endok == false || end == 0 || m_year <= end)) {
			node->setNodeMask(~0);
		}
		// out-of-range
		else {
			node->setNodeMask(0);
		}
	}
}
