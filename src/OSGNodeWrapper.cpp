#include "OSGNodeWrapper.h"

#include "Core/Util.h"
#include <iostream>
#include <osg/Group>
#include <QDebug>

OSGNodeWrapper::OSGNodeWrapper(QObject * parent)
	: QObject(parent),
	m_time_enabled(true),
	m_year(0)
{
}

void OSGNodeWrapper::setRoot(osg::ref_ptr<osg::Node> node)
{
	m_root = node;
}

osg::Node *OSGNodeWrapper::getRoot() const
{
	return m_root;
}

void OSGNodeWrapper::setNodeYear(osg::Node *node, int year, bool begin)
{
	std::string prop;
	if (begin) prop = "yearBegin";
	else prop = "yearEnd";

	if (year == 0) {
		osg::UserDataContainer *cont = node->getUserDataContainer();
		if (cont) {
			cont->removeUserObject(cont->getUserObjectIndex(prop));
		}
	}
	else {
		node->setUserValue(prop, year);
	}
	applyTime(node);
	emit sNodeYearChanged(node, year, begin);
}

int OSGNodeWrapper::getNodeYear(osg::Node *node, bool begin, bool *exists)
{
	int year;
	std::string prop = begin ? "yearBegin" : "yearEnd";

	bool ok = node->getUserValue(prop, year);
	if (exists) *exists = ok;
	if (!ok || year == 0) {
		return 0;
	}
	return year;
}

void OSGNodeWrapper::enableTime(bool enable)
{
	m_time_enabled = enable;
	applyTime();
}

void OSGNodeWrapper::setYear(int year)
{
	m_year = year;
	applyTime();
}

void OSGNodeWrapper::applyTime(osg::Node *node)
{
	if (m_root == nullptr) return;
	if (node == nullptr) {
		node = m_root;
	}

	int year = 0;
	if (m_time_enabled) year = m_year;
	TimeMaskVisitor::touchRecursive(*m_root, year);
}

//void OSGNodeWrapper::addChild(osg::Group * group, osg::Node *node, index)
//{
//}
//
//void OSGNodeWrapper::removeChild(osg::Group * group, size_t index)
//{
//}

TimeInitVisitor::TimeInitVisitor(OSGNodeWrapper *base)
	: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
	m_base(base)
{
	setNodeMaskOverride(~0);
}

void TimeInitVisitor::apply(osg::Node & node)
{
	touch(m_base, &node);
	traverse(node);
}

//void TimeInitVisitor::apply(osg::Group &group)
//{
//	for (uint i = 0; i < group.getNumChildren(); i++) {
//		touch(m_group, m_model, group.getChild(i));
//	}
//	traverse(group);
//}

void TimeInitVisitor::touch(OSGNodeWrapper *base, osg::Node *node) {
	int begin, end;
	bool match = Util::nodeTimeInName(node->getName(), &begin, &end);
	if (match) {
		base->setNodeYear(node, begin, true);
		base->setNodeYear(node, end, false);
	}
}

TimeGetVisitor::TimeGetVisitor()
	: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
	setNodeMaskOverride(~0);
}

void TimeGetVisitor::apply(osg::Node & node)
{
	// look for ModelNodes
	// add the years to the set
	int begin = OSGNodeWrapper::getNodeYear(&node, true);
	int end = OSGNodeWrapper::getNodeYear(&node, false);
	if (begin) m_begins.insert(begin);
	if (end) m_ends.insert(end + 1);
	traverse(node);
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
	m_year(year)
{
	setNodeMaskOverride(~0);
}

void TimeMaskVisitor::apply(osg::Node &node)
{
	touch(node, m_year);
	traverse(node);
}

void TimeMaskVisitor::touch(osg::Node &node, int year)
{
	int begin = OSGNodeWrapper::getNodeYear(&node, true);
	int end = OSGNodeWrapper::getNodeYear(&node, false);

	// see everything
	if (year == 0) {
		node.setNodeMask(~0);
	}
	// in-range
	// If begin isn't there, or begin is 0, or year >= begin, then ok
	else if ((begin == 0 || year >= begin) &&
		(end == 0 || year <= end)) {
		node.setNodeMask(~0);
	}
	// out-of-range
	else {
		node.setNodeMask(0);
	}
}

void TimeMaskVisitor::touchRecursive(osg::Node & node, int year)
{
	TimeMaskVisitor v(year);
	v.apply(node);
}


DebugVisitor::DebugVisitor()
	: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
	m_tabs(0)
{
}

void DebugVisitor::apply(osg::Group &node)
{
	print(node);

	m_tabs++;
	traverse(node);
	m_tabs--;
}

void DebugVisitor::apply(osg::Node & node)
{
	print(node);
	traverse(node);
}

void DebugVisitor::print(osg::Node &node) const
{
	std::cout << std::string(m_tabs, '\t');
	std::cout << node.className() << " " << node.getName() << '\n';

}
