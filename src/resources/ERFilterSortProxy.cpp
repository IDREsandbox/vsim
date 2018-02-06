#include "resources/ERFilterSortProxy.h"
#include <QDebug>
#include <set>

#include "resources/ECategory.h"
#include "resources/EResource.h"
#include "resources/ECategoryGroup.h"
#include "CheckableListProxy.h"
#include "GroupModel.h"

ERFilterSortProxy::ERFilterSortProxy(Group *base)
	: m_base(nullptr),
	m_sort_by(NONE),
	m_category_filters(nullptr),
	m_type_filters(nullptr),
	m_show_global(true),
	m_show_local(true),
	m_enable_range(true),
	m_enable_years(true),
	m_show_all(false)
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

void ERFilterSortProxy::setCategories(CheckableListProxy * cats)
{
	if (m_category_filters != nullptr) disconnect(m_category_filters, 0, this, 0);
	m_category_filters = cats;

	if (!m_category_filters) return;

	// add -> do nothing
	connect(cats, &QAbstractItemModel::rowsInserted, this,
		[this](const QModelIndex &parent, int first, int last) {
		for (int i = first; i <= last; i++) {
			updateCategorySet(i);
		}
	});

	// remove -> remove from set
	connect(cats, &QAbstractItemModel::rowsAboutToBeRemoved, this,
		[this, cats](const QModelIndex &parent, int first, int last) {

		for (int i = first; i <= last; i++) {
			ECategory *cat = GroupModel::get<ECategory*>(cats, i);
			if (!cat) continue;
			m_categories_enabled.erase(cat);
		}
		rescan();
	});

	// change -> add/remove accordingly
	connect(cats, &QAbstractItemModel::dataChanged, this,
		[this, cats](const QModelIndex &topLeft,
			const QModelIndex &bottomRight,
			const QVector<int> &roles = QVector<int>()) {
		bool ok = false;
		// look for the checkbox role
		for (auto role : roles) {
			if (role == Qt::ItemDataRole::CheckStateRole) {
				ok = true;
				break;
			}
		}
		if (!ok) return;

		for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
			updateCategorySet(i);
		}
		rescan();
	});

	// reset
	connect(cats, &QAbstractItemModel::modelReset, this,
		[this, cats]() {
		for (int i = 1; i < cats->rowCount(); i++) {
			updateCategorySet(i);
		}
		rescan();
	});
}

void ERFilterSortProxy::setFiletypes(CheckableListProxy * types)
{
	if (m_type_filters != nullptr) disconnect(m_type_filters, 0, this, 0);
	m_type_filters = types;
}

void ERFilterSortProxy::showLocal(bool local)
{
	m_show_local = local;
}

void ERFilterSortProxy::showGlobal(bool global)
{
	m_show_global = global;
}

//ERFilterSortProxy::SortBy ERFilterSortProxy::getSortMethod() const
//{
//	return m_sort_by;
//}

void ERFilterSortProxy::rescan()
{
	clear();

	if (!m_base) return;

	for (int i = 0; i < (int)m_base->getNumChildren(); i++) {
		checkAndAdd(i);
	}
}

bool ERFilterSortProxy::accept(osg::Node *node)
{
	EResource *res = dynamic_cast<EResource*>(node);
	if (!res) return false;

	// check all
	if (m_show_all) return true;

	// check categories
	if (m_category_filters && res->getCategory() != nullptr) {
		bool cat_ok = (m_categories_enabled.find(res->getCategory()) != m_categories_enabled.end());
		if (!cat_ok) return false;
	}

	// check filetype
	if (m_type_filters) {
		//if (!checkTitle(res->getResourceName())) return false;
	}

	// check title

	// check distance
	//m_enable_range;

	// check years
	//m_enable_years;

	// check global/local
	bool global = res->getGlobal();
	if (!m_show_global && global) return false;
	if (!m_show_local && !global) return false;

	return true;
}

bool ERFilterSortProxy::lessThan(int left, int right)
{
	if (m_sort_by == NONE) {
		return left < right;
	}
	else if (m_sort_by == ALPHABETICAL) {
		EResource *lres = dynamic_cast<EResource*>(m_base->child(left));
		EResource *rres = dynamic_cast<EResource*>(m_base->child(right));
		if (!lres || !rres) return false;
		return lres->getResourceName() < rres->getResourceName();
	}
	return false;
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

void ERFilterSortProxy::enableRange(bool enable)
{
	m_enable_range = enable;
}

void ERFilterSortProxy::enableYears(bool enable)
{
	m_enable_years = enable;
}

void ERFilterSortProxy::setTitleSearch(const std::string & title)
{
	m_title_search = title;
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
		track(m_base->child(i));
		checkAndAdd(i);
	}

	// listen to new
	//connect(base, &Group::sNew, this, [this](int index) {
	//	// fix up indices
	//	// everything after the new item is shifted right 1
	//	for (int i = 0; i < m_map_to_base.size(); i++) {
	//		if (m_map_to_base[i] >= index) {
	//			m_map_to_base[i] += 1;
	//		}
	//	}
	//	track(m_base->child(index));
	//	checkAndAdd(index);
	//});
	// listen to new set
	connect(base, &Group::sInsertedSet, this,
		[this](const std::map<int, osg::Node*> &nodes) {
		// fix up indices
		// set of indices added O(nlogn)
		std::set<int> new_indices;
		for (auto &pair : nodes) {
			new_indices.insert(pair.first);
		}
		// O(n)
		// array of change [0, 0, 1, 2, 2, 2]
		int old_size = m_base->getNumChildren() - nodes.size();
		std::vector<int> shift_array(old_size, 0);
		int shift = 0;
		for (size_t i = 0; i < shift_array.size(); i++) {
			if (new_indices.find(i) != new_indices.end()) {
				shift++;
			}
			shift_array[i] = shift;
		}
		// O(n)
		// update old indices
		for (size_t i = 0; i < m_map_to_base.size(); i++) {
			size_t old_index = m_map_to_base[i];
			m_map_to_base[i] = old_index + shift_array[old_index];
		}

		// filter
		std::vector<osg::Node*> nodes_filtered;
		for (auto &pair : nodes) {
		}

		// sort
		std::vector<osg::Node*> nodes_sorted;

		// merge
		// O(n), map O(nlogn)
		std::map<int, osg::Node*> nodes_inserted = merge();

		emit sInsertedSet(indexes_added);
	});
	// listen to delete
	connect(base, &Group::sInsertedSet, this,
		[this](const std::map<int, osg::Node*> &) {

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
//
//void ERFilterSortProxy::setCategories(ECategoryGroup * categories)
//{
//	if (m_categories) disconnect(m_categories, 0, this, 0);
//	m_categories = categories;
//
//	// add all of them
//	for (int i = 0; i < (int)m_categories->getNumChildren(); i++) {
//		ECategory *cat = m_categories->category(i);
//		if (!cat) continue;
//		m_enable_categories.insert(cat);
//	}
//
//	// react to new and delete
//	connect(categories, &Group::sNew, this,
//		[this](int index) {
//		ECategory *cat = m_categories->category(index);
//		if (!cat) return;
//		m_enable_categories.insert(cat);
//	});
//	connect(categories, &Group::sDelete, this,
//		[this](int index) {
//		ECategory *cat = m_categories->category(index);
//		if (!cat) return;
//		m_enable_categories.insert(cat);
//	});
//	rescan();
//}

osg::Node *ERFilterSortProxy::child(unsigned int index) const
{
	if (!m_base) return nullptr;
	if (index >= m_map_to_base.size()) {
		qWarning() << "proxy child out of range" << index << this;
		return nullptr;
	}
	return m_base->child(m_map_to_base[index]);
}

unsigned int ERFilterSortProxy::getNumChildren() const
{
	if (!m_base) return 0;
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
	std::function<bool(int, int)> m_func = [this](int l, int r) { return lessThan(l, r); };
	auto it = std::lower_bound(m_map_to_base.begin(), m_map_to_base.end(), base_index, m_func);
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

void ERFilterSortProxy::updateCategorySet(int model_row)
{
	if (model_row == 0) return;
	ECategory *cat = GroupModel::get<ECategory*>(m_category_filters, model_row);
	if (!cat) return;
	Qt::CheckState checked = 
		m_category_filters->data(
			m_category_filters->index(model_row, 0), Qt::ItemDataRole::CheckStateRole)
		.value<Qt::CheckState>();

	if (checked == Qt::Checked) {
		if (m_categories_enabled.find(cat) == m_categories_enabled.end()) {
			m_categories_enabled.insert(cat);
		}
	}
	else {
		int del = m_categories_enabled.erase(cat);
	}
}

bool ERFilterSortProxy::checkTitle(const std::string & s) const
{
	if (!m_type_filters) return true;

	// do some regex

	return true;
}

int ERFilterSortProxy::indexOf(const osg::Node *node) const
{
	if (!m_base) return -1;
	for (int i = 0; i < (int)getNumChildren(); i++) {
		if (child(i) == node) return i;
	}
	return -1;
}

void ERFilterSortProxy::track(osg::Node *node)
{
	EResource *res = dynamic_cast<EResource*>(node);
	if (!res) return;

	// listen to the new item
	connect(res, &EResource::sResourceNameChanged, this, [this, res]() { onResourceChange(res); });
	connect(res, &EResource::sCategoryChanged, this, [this, res]() { onResourceChange(res); });
	connect(res, &EResource::sGlobalChanged, this, [this, res]() { onResourceChange(res); });
}
