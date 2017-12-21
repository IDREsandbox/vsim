#include "resources/ERFilterSortProxy.h"
#include <QDebug>

ERFilterSortProxy::ERFilterSortProxy(Group *base)
	: m_base(nullptr),
	m_sort_by(NONE),
	m_enable_all(false),
	m_filter_global(SHOW_BOTH),
	m_enable_local(false)
{
	setBase(base);
	sortBy(ALPHABETICAL);
}

void ERFilterSortProxy::sortBy(SortBy method)
{
	if (m_sort_by == method) return;
	m_sort_by = method;
	m_included.clear();
	m_title_hashmap.clear();
	m_title_map.clear();
	if (method == NONE) {
		rescan();
	}
	else if (method == ALPHABETICAL) {
		rescan();
	}
	else if (method == DISTANCE) {

	}
}

//ERFilterSortProxy::SortBy ERFilterSortProxy::getSortMethod() const
//{
//	return m_sort_by;
//}

void ERFilterSortProxy::rescan()
{
	if (m_sort_by == NONE) {
		m_included.clear();
		for (int i = 0; i < (int)m_base->getNumChildren(); i++) {
			EResource *res = dynamic_cast<EResource*>(m_base->child(i));
			if (!res) return;
			if (accept(res)) add(res);
		}
	}
	else if (m_sort_by == ALPHABETICAL) {
		for (int i = 0; i < (int)m_base->getNumChildren(); i++) {
			EResource *res = dynamic_cast<EResource*>(m_base->child(i));
			if (!res) return;
			checkAndAdd(res);
		}
	}
}

bool ERFilterSortProxy::accept(EResource *res)
{
	qDebug() << "accept?";
	// check all
	if (m_enable_all) return true;

	// check categories
	if (!m_enable_categories.empty()) {
		bool cat_ok = (m_enable_categories.find(res->category()) != m_enable_categories.end());
		if (!cat_ok) return false;
	}

	// check filetype

	// check titles

	// check global/local
	bool global = res->getGlobal();
	qDebug() << "checkgl" << m_filter_global << global << res;
	if (m_filter_global == SHOW_GLOBAL && !global) return false;
	if (m_filter_global == SHOW_LOCAL && global) return false;

	return true;
}

void ERFilterSortProxy::checkAndAdd(EResource * res)
{
	bool ok = accept(res);
	if (ok) {
		qDebug() << "resource" << res << QString::fromStdString(res->getResourceName()) << "accepted";
		add(res);
	}
	else {
		qDebug() << "resource" << res << QString::fromStdString(res->getResourceName()) << "rejected";
		remove(res);
	}
}

void ERFilterSortProxy::addCategory(ECategory * cat)
{
}

void ERFilterSortProxy::removeCategory(ECategory * cat)
{
}

void ERFilterSortProxy::addFiletype(const std::string & extension)
{
}

void ERFilterSortProxy::removeFiletype(const std::string & extension)
{
}

void ERFilterSortProxy::addTitleSearch(const std::string & extension)
{
}

void ERFilterSortProxy::removeTitleSearch(const std::string & extension)
{
}

void ERFilterSortProxy::filterGlobal(FilterGlobal what)
{
	if (m_filter_global == what) return;
	m_filter_global = what;
	rescan();
}

void ERFilterSortProxy::enableRange(bool enable)
{
}

void ERFilterSortProxy::setBase(Group *base)
{
	if (m_base != nullptr) disconnect(m_base, 0, this, 0);
	qDebug() << "setting base to" << base;
	// remove everything signal
	for (unsigned int i = 0; i < getNumChildren(); i++) {
		remove(dynamic_cast<EResource*>(child(i)));
	}

	m_base = base;
	if (base == nullptr) return;

	// track all of the existing items
	for (unsigned int i = 0; i < base->getNumChildren(); i++) {
		track(dynamic_cast<EResource*>(m_base->child(i)));
	}

	// listen to new
	connect(base, &Group::sNew, this, [this](int index) {
		EResource *res = dynamic_cast<EResource*>(m_base->child(index));
		if (!res) return;
		track(res);
	});
}

bool ERFilterSortProxy::insertChild(unsigned int index, Node * child)
{
	//qWarning() << "Insertion into a group proxy - item added to end";
	return m_base->addChild(child);
}

bool ERFilterSortProxy::removeChildren(unsigned int index, unsigned int numChildrenToRemove)
{
	std::vector<EResource*> delete_me;
	auto it = m_title_map.begin();
	std::advance(it, index);
	for (unsigned int i = 0; i < numChildrenToRemove; i++) {
		delete_me.push_back(it->second);
	}

	for (EResource *res : delete_me) {
		m_base->removeChild(res);
	}
	return true;
}

osg::Node *ERFilterSortProxy::child(unsigned int index)
{
	if (m_sort_by == NONE) {
		return m_included[index];
	}
	else if (m_sort_by == ALPHABETICAL) {
		auto it = m_title_map.begin();
		std::advance(it, index);
		return it->second;
	}
	return nullptr;
}

unsigned int ERFilterSortProxy::getNumChildren() const
{
	if (m_sort_by == NONE) {
		return m_included.size();
	}
	else if (m_sort_by == ALPHABETICAL) {
		return m_title_map.size();
	}
	return 0;
}

void ERFilterSortProxy::setPosition(osg::Vec3f pos)
{
	m_position = pos;
}

EResource * ERFilterSortProxy::getResource(int i)
{
	if (i >= (int)getNumChildren() || i < 0) return nullptr;
	return dynamic_cast<EResource*>(child(i));
}

void ERFilterSortProxy::debug()
{
	for (unsigned int i = 0; i < getNumChildren(); i++) {
		EResource *res = getResource(i);
		if (!res) qDebug() << i << nullptr;
		else qDebug() 
			<< i 
			<< QString::fromStdString(res->getResourceName()) 
			<< "g:" 
			<< res->getGlobal();
	}
}

void ERFilterSortProxy::add(EResource *res)
{
	if (m_sort_by == NONE) {
		int index = m_included.size();
		m_included.push_back(res);
		emit sNew(index);
	}
	else if (m_sort_by == ALPHABETICAL) {
		// find
		auto it = m_title_hashmap.find(res);

		// already in the map
		if (it != m_title_hashmap.end()) return;

		// insert into our maps
		auto elem_already = m_title_map.insert({res->getResourceName(), res});
		m_title_hashmap.insert({res, elem_already.first});

		// emit add signal... how to get the index?
		qDebug() << "er filter sort proxy add" << QString::fromStdString(res->getResourceName());
		emit sNew(std::distance(m_title_map.begin(), elem_already.first));
	}
}

void ERFilterSortProxy::remove(EResource *res)
{
	if (m_sort_by == NONE) {
		auto it = std::find(m_included.begin(), m_included.end(), res);
		int index = it - m_included.begin();
		m_included.erase(it);
		emit sDelete(index);
	}
	else if (m_sort_by == ALPHABETICAL) {
		auto it = m_title_hashmap.find(res);

		// not in the map
		if (it == m_title_hashmap.end()) return;

		int index = std::distance(m_title_map.begin(), it->second);
		// remove from maps
		m_title_map.erase(it->second);
		m_title_hashmap.erase(it);

		// emit remove signal
		emit sDelete(index);
	}
}

void ERFilterSortProxy::remap(EResource * res)
{
	remove(res);
	checkAndAdd(res);
}

bool ERFilterSortProxy::inMap(EResource * res)
{
	if (m_sort_by == NONE) {
		return std::find(m_included.begin(), m_included.end(), res) != m_included.end();
	}
	else if (m_sort_by == ALPHABETICAL) {
		auto it = m_title_hashmap.find(res);
		if (it == m_title_hashmap.end()) return false;
		return false;
	}
	return false;
}

int ERFilterSortProxy::indexOf(EResource * res)
{
	if (m_sort_by == NONE) {
		// this one is super hacky
		//for (auto i = foo.begin(); i != foo.end(); ++i) {
		//}
	}
	else if (m_sort_by == ALPHABETICAL) {
		// there must be a better way... but whatever
		auto it = m_title_hashmap.find(res);
		if (it == m_title_hashmap.end()) return -1;
		int index = std::distance(m_title_map.begin(), it->second);
	}
	return 0;
}

int ERFilterSortProxy::baseIndexOf(EResource *res)
{
	return m_base->getChildIndex(res);
}

void ERFilterSortProxy::track(EResource * res)
{
	qDebug() << "tracking" << QString::fromStdString(res->getResourceName());
	// listen to the new item
	connect(res, &EResource::sResourceNameChanged, this, [this, res](const std::string& new_name) {
		if (m_sort_by != ALPHABETICAL) return;
		// by now the index can be completely messed up...
		remap(res);
	});
	connect(res, &EResource::sCategoryChanged, this, [this, res]() { checkAndAdd(res); });
	connect(res, &EResource::sGlobalChanged, this, [this, res]() { checkAndAdd(res); });
	checkAndAdd(res);
}
