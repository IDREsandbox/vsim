#include "ModelGroup.h"
#include <QDebug>
#include <regex>
#include "Util.h"

ModelGroup::ModelGroup()
	: m_year(0),
	m_time_enabled(false)
{
	enableTime(true);
	//qDebug() << "new model table" << m_data_table.get();
}

void ModelGroup::merge(ModelGroup *other)
{
	for (uint i = 0; i < other->getNumChildren(); i++) {
		addChild(other->getChild(i));
	}
}

void ModelGroup::setNodeYear(osg::Node *node, int year, bool begin)
{
	std::string prop;
	if (begin) prop = "yearBegin";
	else prop = "yearEnd";

	if (year == 0) {
		osg::UserDataContainer *cont = node->getUserDataContainer();
		cont->removeUserObject(cont->getUserObjectIndex(prop));
	}
	else {
		node->setUserValue(prop, year);
	}
	emit sNodeYearChanged(node, year, begin);
}

int ModelGroup::getYear() const
{
	return m_year;
}

void ModelGroup::setYear(int year)
{
	m_year = year;

	// make a year traverser
	if (m_time_enabled) {
		TimeMaskVisitor v(year);
		accept(v);
	}

	emit sYearChange(year);
}

bool ModelGroup::timeEnabled() const
{
	return m_time_enabled;
}

void ModelGroup::enableTime(bool enable)
{
	if (m_time_enabled == enable) return;

	m_time_enabled = enable;
	emit sTimeEnableChange(enable);

	if (m_time_enabled) {
		TimeMaskVisitor v(getYear());
		accept(v);
	}
	else {
		TimeMaskVisitor v(0);
		accept(v);
	}
}

std::set<int> ModelGroup::getKeyYears()
{
	TimeGetVisitor v;
	accept(v);
	std::set<int> years = v.results();

	if (years.empty()) return years;

	return years;
}

bool ModelGroup::addChild(osg::Node *child)
{
	bool ok = Group::addChild(child);
	if (!ok) return false;

	// time init the child
	TimeInitVisitor v(this); // check for T: start end
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

TimeInitVisitor::TimeInitVisitor(ModelGroup * group)
	: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
	m_group(group)
{
}

void TimeInitVisitor::apply(osg::Group &group)
{
	for (uint i = 0; i < group.getNumChildren(); i++) {
		touch(m_group, group.getChild(i));
	}
	traverse(group);
}

void TimeInitVisitor::touch(ModelGroup *group, osg::Node *node) {
	int begin, end;
	bool match = ModelGroup::nodeTimeInName(node->getName(), &begin, &end);
	if (match) {
		if (begin != 0) {
			group->setNodeYear(node, begin, true);
		}
		if (end != 0) {
			group->setNodeYear(node, end, false);
		}
	}
}

TimeGetVisitor::TimeGetVisitor()
	: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN) {}
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

		if (beginok) m_begins.insert(begin);
		if (endok) m_ends.insert(end + 1);
	}
	traverse(group);
}

std::set<int> TimeGetVisitor::results() const
{
	// add the ends
	std::set<int> out = m_ends;

	// remove the last end (to prevent the apocalypse)
	// should we add (end - 1) to show a range?
	if (!m_ends.empty()) {
		if (m_begins.empty() || *m_ends.rbegin() > *m_begins.rbegin()) {
			int last = *(--out.end());
			out.erase(last);
			out.insert(last - 1);
		}
	}

	// add the begins
	out.insert(m_begins.begin(), m_begins.end());
	return out;
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
	traverse(group);
}
