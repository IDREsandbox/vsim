#include "resources/ERFilterSortProxy.h"
#include <QDebug>

#include "resources/ECategory.h"
#include "resources/EResource.h"
#include "resources/ECategoryGroup.h"

ERFilterSortProxy::ERFilterSortProxy(Group *base)
	: m_base(nullptr),
	m_categories(nullptr),
	m_sort_by(NONE),
	m_enable_all(false),
	m_filter_global(SHOW_BOTH)
{
	setBase(base);
	sortBy(NONE);
}

void ERFilterSortProxy::sortBy(SortBy method)
{
	if (m_sort_by == method) return;
	m_sort_by = method;
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
	clear();

	for (int i = 0; i < (int)m_base->getNumChildren(); i++) {
		checkAndAdd(i);
	}
}

bool ERFilterSortProxy::accept(EResource *res)
{
	// check all
	if (m_enable_all) return true;

	// check categories
	bool cat_ok = (m_enable_categories.find(res->category()) != m_enable_categories.end());
	if (!cat_ok) return false;

	// check filetype

	// check titles

	// check global/local
	bool global = res->getGlobal();
	if (m_filter_global == SHOW_GLOBAL && !global) return false;
	if (m_filter_global == SHOW_LOCAL && global) return false;

	return true;
}

void ERFilterSortProxy::checkAndAdd(int base_index)
{
	EResource *res = dynamic_cast<EResource*>(m_base->child(base_index));
	if (!res) return;
	bool ok = accept(res);
	if (ok) {
		add(base_index);
	}
	else {
		remove(base_index);
	}
}

void ERFilterSortProxy::onResourceChange(EResource * res)
{
	// find the base index
	int base_index = m_base->indexOf(res);

	// readd it
	remove(base_index);
	checkAndAdd(base_index);
}

void ERFilterSortProxy::addCategory(ECategory * cat)
{
	m_enable_categories.insert(cat);
	rescan();
}

void ERFilterSortProxy::removeCategory(ECategory * cat)
{
	m_enable_categories.erase(cat);
	rescan();
}

void ERFilterSortProxy::allCategories(bool all)
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
	qInfo() << "Setting proxy <<" << this << "base to" << base;

	m_base = base;
	if (base == nullptr) return;

	// remove everything signal
	clear();

	// track all of the existing items
	for (unsigned int i = 0; i < base->getNumChildren(); i++) {
		track(i);
		checkAndAdd(i);
	}

	// listen to new
	connect(base, &Group::sNew, this, [this](int index) {
		// fix up indices
		// everything after the new item is shifted right 1
		for (int i = 0; i < m_map_to_base.size(); i++) {
			if (m_map_to_base[i] >= index) {
				m_map_to_base[i] += 1;
			}
		}
		track(index);
		checkAndAdd(index);
	});
	connect(base, &Group::sDelete, this, [this](int index) {
		for (int i = m_map_to_base.size() - 1; i >= 0; i--) {
			if (m_map_to_base[i] == index) {
				// this item got deleted
				m_map_to_base.erase(m_map_to_base.begin() + i);
				emit sDelete(i);
			}
			else if (m_map_to_base[i] > index) {
				m_map_to_base[i] -= 1;
			}
		}
	});
}

void ERFilterSortProxy::setCategories(ECategoryGroup * categories)
{
	if (m_categories) disconnect(m_categories, 0, this, 0);
	m_categories = categories;

	// add all of them
	for (int i = 0; i < (int)m_categories->getNumChildren(); i++) {
		ECategory *cat = m_categories->category(i);
		if (!cat) continue;
		m_enable_categories.insert(cat);
	}

	// react to new and delete
	connect(categories, &Group::sNew, this,
		[this](int index) {
		ECategory *cat = m_categories->category(index);
		if (!cat) return;
		m_enable_categories.insert(cat);
	});
	connect(categories, &Group::sDelete, this,
		[this](int index) {
		ECategory *cat = m_categories->category(index);
		if (!cat) return;
		m_enable_categories.insert(cat);
	});
	rescan();
}

osg::Node *ERFilterSortProxy::child(unsigned int index) const
{
	if (index >= m_map_to_base.size()) {
		qWarning() << "proxy get child out of range";
		return nullptr;
	}
	return m_base->child(m_map_to_base[index]);
}

unsigned int ERFilterSortProxy::getNumChildren() const
{
	return m_map_to_base.size();
}

void ERFilterSortProxy::setPosition(osg::Vec3f pos)
{
	m_position = pos;
}

EResource * ERFilterSortProxy::getResource(int i) const
{
	if (i >= (int)getNumChildren() || i < 0) return nullptr;
	return dynamic_cast<EResource*>(child(i));
}

void ERFilterSortProxy::debug()
{
	for (unsigned int i = 0; i < getNumChildren(); i++) {
		EResource *res = getResource(i);
		if (!res) qInfo() << i << nullptr;
		else qInfo()
			<< "Debugging proxy:\n"
			<< i 
			<< QString::fromStdString(res->getResourceName()) 
			<< "g:" 
			<< res->getGlobal();
	}
}

void ERFilterSortProxy::add(int base_index)
{
	EResource *res = dynamic_cast<EResource*>(m_base->child(base_index));
	std::function<bool(int, int)> sort_func;
	if (m_sort_by == NONE) {
		sort_func = std::less<int>();
	}
	else if (m_sort_by == ALPHABETICAL) {
		sort_func = [this, res](int left, int right) {
			EResource *lres = dynamic_cast<EResource*>(m_base->child(left));
			EResource *rres = dynamic_cast<EResource*>(m_base->child(right));
			//qDebug() << "comparison:"
			//	<< left << QString::fromStdString(lres->getResourceName())
			//	<< right << QString::fromStdString(rres->getResourceName());
			return lres->getResourceName() < rres->getResourceName();
		};
	}
	auto it = std::lower_bound(m_map_to_base.begin(), m_map_to_base.end(), base_index, sort_func);
	if (it != m_map_to_base.end() && *it == base_index) {
		qWarning() << "base index" << *it << "already added to sorted proxy";
		return;
	}
	int new_index = it - m_map_to_base.begin();
	m_map_to_base.insert(it, base_index);

	emit sNew(new_index);
}

void ERFilterSortProxy::remove(int base_index)
{
	auto it = std::find(m_map_to_base.begin(), m_map_to_base.end(), base_index);
	if (it == m_map_to_base.end()) return;
	int index = it - m_map_to_base.begin();
	m_map_to_base.erase(it);
	emit sDelete(index);
}

void ERFilterSortProxy::clear()
{
	for (int i = m_map_to_base.size() - 1; i >= 0; i--) {
		m_map_to_base.pop_back();
		emit sDelete(i);
	}
}

int ERFilterSortProxy::indexOf(const osg::Node *node) const
{
	for (int i = 0; i < (int)getNumChildren(); i++) {
		if (child(i) == node) return i;
	}
	return -1;
}

void ERFilterSortProxy::track(int base_index)
{
	EResource *res = dynamic_cast<EResource*>(m_base->child(base_index));
	if (!res) return;

	// listen to the new item
	connect(res, &EResource::sResourceNameChanged, this, [this, res]() { onResourceChange(res); });
	connect(res, &EResource::sCategoryChanged, this, [this, res]() { onResourceChange(res); });
	connect(res, &EResource::sGlobalChanged, this, [this, res]() { onResourceChange(res); });
}
