#include "resources/ERFilterSortProxy.h"
#include <QDebug>
#include <set>
#include <unordered_set>

#include "resources/ECategory.h"
#include "resources/EResource.h"
#include "resources/ECategoryGroup.h"
#include "CheckableListProxy.h"
#include "VecUtil.h"
#include "Util.h"
#include "GroupModelTemplate.h"

ERFilterSortProxy::ERFilterSortProxy(TGroup<EResource> *base)
	: m_base(nullptr),
	m_sort_by(ER::SortBy::NONE),
	m_category_filters(nullptr),
	m_type_filters(nullptr),
	m_show_global(true),
	m_show_local(true),
	m_enable_range(false),
	m_enable_years(true),
	m_show_all(false),
	m_radius(1.0)
{
	setBase(base);
	sortBy(ER::SortBy::NONE);
}

void ERFilterSortProxy::setBase(TGroup<EResource> *base)
{
	if (m_base != nullptr) disconnect(m_base, 0, this, 0);

	m_base = base;
	if (base == nullptr) return;

	reload();

	// listen to new set
	//connect(base, &GroupSignals::sInserted, this,
	//	[this](auto stuff) {

	//});
	connect(base, &GroupSignals::sInsertedMulti, this,
		[this](const std::vector<size_t> &ins) {
		
		// fix indices
		std::set<size_t> set(ins.begin(), ins.end());
		m_map_to_base = VecUtil::fixIndices(m_map_to_base, set);

		checkAndInsertSet(set);
	});

	// listen to delete
	connect(base, &GroupSignals::sRemovedMulti, this,
		[this](const std::vector<size_t> &removals) {

		std::unordered_set<size_t> removed(removals.begin(), removals.end());

		// what gets removed from our local map
		std::vector<size_t> local_removals;
		for (size_t i = 0; i < m_map_to_base.size(); i++) {
			if (removed.find(m_map_to_base[i]) != removed.end()) {
				local_removals.push_back(i);
			}
		}

		// fix up indices
		std::set<size_t> removals_set(removals.begin(), removals.end());
		m_map_to_base = VecUtil::fixIndicesRemove(m_map_to_base, removals_set);

		removeAndSignal(local_removals);
	});
	connect(base, &GroupSignals::sReset, this,
		[this]() {
		reload();
	});
	connect(base, &GroupSignals::sEdited, this,
		[this](const std::set<size_t> &indices) {

		// decompose into items to remove and items to insert
		recheck(indices);

		auto new_edited = baseToLocal(indices);
		std::set<size_t> uset(new_edited.begin(), new_edited.end());
		if (new_edited.size() > 0) emit sEdited(uset);
	});
}

void ERFilterSortProxy::sortBy(ER::SortBy method)
{
	if (m_sort_by == method) return;
	m_sort_by = method;

	reload();
	emit sSortByChanged(method);
	//if (method == NONE) {
	//	rescan();
	//}
	//else if (method == TITLE) {
	//	rescan();
	//}
	//else if (method == DISTANCE) {

	//}
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
			ECategory *cat = TGroupModel<ECategory>::get(cats, i);
			if (!cat) continue;
			m_categories_enabled.erase(cat);
		}
		reload();
	});

	// change -> add/remove accordingly
	connect(cats, &QAbstractItemModel::dataChanged, this,
		[this, cats](const QModelIndex &topLeft,
			const QModelIndex &bottomRight,
			const QVector<int> &roles = QVector<int>()) {

		// look for the checkbox role
		if (std::find(roles.begin(), roles.end(), Qt::CheckStateRole) == roles.end()) {
			return;
		}

		for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
			updateCategorySet(i);
		}
		reload();
	});

	// reset
	connect(cats, &QAbstractItemModel::modelReset, this,
		[this, cats]() {
		for (int i = 1; i < cats->rowCount(); i++) {
			updateCategorySet(i);
		}
		reload();
	});

}

void ERFilterSortProxy::setFiletypes(CheckableListProxy * types)
{
	if (m_type_filters != nullptr) disconnect(m_type_filters, 0, this, 0);
	m_type_filters = types;
	reload();
}

void ERFilterSortProxy::showLocal(bool local)
{
	m_show_local = local;
	reload();
}

void ERFilterSortProxy::showGlobal(bool global)
{
	m_show_global = global;
	reload();
}

//ERFilterSortProxy::SortBy ERFilterSortProxy::getSortMethod() const
//{
//	return m_sort_by;
//}

//void ERFilterSortProxy::rescan()
//{
//	clear();
//
//	if (!m_base) return;
//
//	for (int i = 0; i < (int)m_base->getNumChildren(); i++) {
//		checkAndAdd(i);
//	}
//}

bool ERFilterSortProxy::accept(EResource *res)
{
	if (!res) return false;

	// check all
	if (m_show_all) return true;

	// check categories
	if (m_category_filters && res->category() != nullptr) {
		bool cat_ok = (m_categories_enabled.find(res->category()) != m_categories_enabled.end());
		if (!cat_ok) return false;
	}

	// check filetype
	if (m_type_filters) {
		//if (!checkTitle(res->getResourceName())) return false;
	}

	// check title

	// check distance
	if (!res->getGlobal() && m_enable_range) {
		//bool in_range = m_in_range.find(res) != m_in_range.end();
		if (m_in_range.find(res) == m_in_range.end()) {
			return false;
		}
		//bool overlap = Util::spheresOverlap(m_position, m_radius,
		//	res->getPosition(), res->getLocalRange());
		//if (!overlap) return false;
		//if (!overlap) {
		//	m_in_range.erase(res);
		//	return false;
		//}
	}

	//m_enable_range;

	// check years
	//m_enable_years;

	// check global/local
	bool global = res->getGlobal();
	if (!m_show_global && global) return false;
	if (!m_show_local && !global) return false;

	return true;
}

bool ERFilterSortProxy::lessThan(size_t left, size_t right)
{
	if (m_sort_by == ER::SortBy::TITLE) {
		EResource *lres = m_base->child(left);
		EResource *rres = m_base->child(right);
		if (!lres || !rres) return false;
		return lres->getResourceName() < rres->getResourceName();
	}
	return left < right;
}

std::vector<int> ERFilterSortProxy::indicesOf(const std::vector<EResource*>& resources, bool blanks)
{
	if (resources.size() == 0) return {};
	std::vector<int> out;
	std::unordered_map<EResource*, int> res_to_index;
	for (size_t i = 0; i < size(); i++) {
		EResource *res = child(i);
		res_to_index[res] = i;
	}

	for (EResource *res : resources) {
		auto it = res_to_index.find(res);
		if (it != res_to_index.end()) {
			out.push_back(it->second);
		}
		else if (blanks) {
			out.push_back(-1);
		}
	}
	return out;
}

//ERFilterSortProxy::LessThanFunc ERFilterSortProxy::lessThanFunc() const
//{
//	if (m_sort_by == TITLE) {
//		return ERFilterSortProxy::lessThanAlphabetical;
//	}
//	return nullptr;
//}
//
//bool ERFilterSortProxy::lessThanDistance(EResource * left, EResource * right)
//{
//	return false;
//}
//
//bool ERFilterSortProxy::lessThanAlphabetical(EResource * left, EResource * right)
//{
//	EResource *lres = dynamic_cast<EResource*>(left);
//	EResource *rres = dynamic_cast<EResource*>(right);
//	if (!lres || !rres) return false;
//	return lres->getResourceName() < rres->getResourceName();
//}

//void ERFilterSortProxy::checkAndAdd(int base_index)
//{
//	EResource *res = dynamic_cast<EResource*>(m_base->child(base_index));
//	if (!res) return;
//	bool ok = accept(res);
//	if (ok) {
//		add(base_index);
//	}
//	else {
//		remove(base_index);
//	}
//}

std::set<size_t> ERFilterSortProxy::baseToLocal(const std::set<size_t> base_indices) const
{
	// now forward the edited signals
	// map from base indices to our indices
	std::unordered_map<size_t, size_t> map;
	for (size_t i = 0; i < m_map_to_base.size(); i++) {
		size_t base_index = m_map_to_base[i];
		map[base_index] = i;
	}
	// set of edited indices (ours)
	std::set<size_t> local;
	for (int base : base_indices) {
		auto it = map.find(base);
		if (it != map.end()) {
			local.insert(it->second);
		}
	}
	return local;
}

void ERFilterSortProxy::recheck(const std::set<size_t>& base_indices)
{
	checkAndRemoveSet(base_indices);
	checkAndInsertSet(base_indices);
}

void ERFilterSortProxy::checkAndInsertSet(const std::set<size_t> &base_indices)
{
	if (base_indices.size() == 0) return;
	std::unordered_set<size_t> already_mapped(m_map_to_base.begin(), m_map_to_base.end());

	// check if nodes are already in this proxy
	// check if node passes the filter
	std::vector<size_t> nodes_filtered;
	for (int index : base_indices) {
		EResource *res = m_base->child(index);
		if (already_mapped.find(index) == already_mapped.end()
			&& accept(res)) {
			nodes_filtered.push_back(index);
		}
	}

	if (nodes_filtered.size() == 0) {
		return;
	}

	// sort new guys
	//std::function<bool(size_t, size_t)> lt = [this](size_t l, size_t r) { return lessThan(l, r); };
	auto lt = [this](size_t l, size_t r) { return lessThan(l, r); };
	std::sort(nodes_filtered.begin(), nodes_filtered.end(), lt);

	// base before
	//qDebug() << "map before";
	//for (int i : m_map_to_base) {
	//	EResource *res = dynamic_cast<EResource*>(m_base->child(i));
	//	qDebug() << i << ":" << res->getResourceName().c_str();
	//}

	// merge
	std::vector<size_t> result;
	std::merge(m_map_to_base.begin(), m_map_to_base.end(),
		nodes_filtered.begin(), nodes_filtered.end(),
		std::back_inserter(result), lt);

	//qDebug() << "map after";
	//for (int i : result) {
	//	EResource *res = dynamic_cast<EResource*>(m_base->child(i));
	//	qDebug() << i << ":" << res->getResourceName().c_str();
	//}

	// where were nodes inserted?
	// compare two vectors, m_map_to_base and result
	//std::vector<std::pair<size_t, EResource*>> insertions;
	std::vector<size_t> indices;
	//std::vector<EResource*> resources;
	size_t old_i = 0;
	for (size_t i = 0; i < result.size(); i++) {
		if (old_i >= m_map_to_base.size() || m_map_to_base[old_i] != result[i]) {
			EResource *new_node = m_base->child(result[i]);
			//insertions.push_back({ i, new_node });
			indices.push_back(i);
			//resources.push_back(new_node);
		}
		else {
			old_i++;
		}
	}

	//qDebug() << "insertions at";
	//for (auto &p: insertions) {
	//	EResource *res = dynamic_cast<EResource*>(p.second);
	//	qDebug() << p.first << ":" << res->getResourceName().c_str();
	//}

	m_map_to_base = result;
	emit sInsertedMulti(indices);
}

void ERFilterSortProxy::checkAndRemoveSet(const std::set<size_t> &base_indices)
{
	if (base_indices.size() == 0) return;
	// check if node passes the filter
	std::set<size_t> nodes_to_remove_base;
	for (int index : base_indices) {
		EResource *node = m_base->child(index);
		if (!accept(node)) {
			nodes_to_remove_base.insert(index);
		}
	}
	if (nodes_to_remove_base.size() == 0) return;

	// convert to local, check if nodes are already in this proxy
	std::set<size_t> nodes_to_remove = baseToLocal(nodes_to_remove_base);
	std::vector<size_t> removals(nodes_to_remove.begin(), nodes_to_remove.end());

	removeAndSignal(removals);
}
void ERFilterSortProxy::onResourceChange(EResource * res)
{
	//// find the base index
	//int base_index = m_base->indexOf(res);

	//// readd it
	//remove(base_index);
	//checkAndAdd(base_index);
}

void ERFilterSortProxy::enableRange(bool enable)
{
	m_enable_range = enable;
	reload();
	emit sUseRangeChanged(enable);
}

void ERFilterSortProxy::enableYears(bool enable)
{
	m_enable_years = enable;
	reload();
	emit sUseYearsChanged(enable);
}

void ERFilterSortProxy::setSearchRadius(float radius)
{
	m_radius = radius;
	setPosition(m_position);

	emit sRadiusChanged(radius);
}

void ERFilterSortProxy::setTitleSearch(const std::string & title)
{
	m_title_search = title;
	reload();
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

EResource *ERFilterSortProxy::child(size_t index) const
{
	if (!m_base) return nullptr;
	if (index >= m_map_to_base.size()) {
		qWarning() << "proxy child out of range" << index << this;
		return nullptr;
	}
	return m_base->child(m_map_to_base[index]);
}

size_t ERFilterSortProxy::size() const
{
	if (!m_base) return 0;
	return m_map_to_base.size();
}

void ERFilterSortProxy::setPosition(osg::Vec3 pos)
{
	//if (m_position == pos) return;

	if (!m_enable_range) return;

	//qDebug() << "hnm?";
	m_position = pos;

	// 1.
	// remove things that are out of range
	// insert things that are in range

	// 2.
	// rescan everything (we have to anyway)
	// apply differences

	std::set<size_t> entered_range; // base indices
	std::set<size_t> left_range; // base indices

	// scan everything check if in range
	for (size_t i = 0; i < m_base->size(); i++) {
		EResource * res = m_base->child(i);

		if (res->getGlobal()) continue;
		res->getPosition();
		res->getLocalRange();

		// two circles overlap
		bool overlap = Util::spheresOverlap(pos, m_radius, res->getPosition(), res->getLocalRange());
		bool was_in_range = m_in_range.find(res) != m_in_range.end();
		//qDebug() << "overlap?" << overlap << res;
		//qDebug() << "m_rad" << m_radius << "range" << res->getLocalRange() << "dist" << (pos - res->getPosition()).length() << "was" << was_in_range;
		// overlap and not in map, then insert it
		if (overlap && !was_in_range) {
			entered_range.insert(i);
			m_in_range.insert(res);
		}
		// overlap and in map, then remove it
		else if (!overlap && was_in_range) {
			left_range.insert(i);
			m_in_range.erase(res);
		}
	}

	// re-check
	checkAndInsertSet(entered_range);
	checkAndRemoveSet(left_range);
}

EResource * ERFilterSortProxy::getResource(int i) const
{
	return child(i);
}

void ERFilterSortProxy::debug()
{
	for (unsigned int i = 0; i < size(); i++) {
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

//void ERFilterSortProxy::add(int base_index)
//{
//	std::function<bool(int, int)> m_func = [this](int l, int r) { return lessThan(l, r); };
//	auto it = std::lower_bound(m_map_to_base.begin(), m_map_to_base.end(), base_index, m_func);
//	if (it != m_map_to_base.end() && *it == base_index) {
//		qWarning() << "base index" << *it << "already added to sorted proxy";
//		return;
//	}
//	int new_index = it - m_map_to_base.begin();
//	m_map_to_base.insert(it, base_index);
//
//	emit sNew(new_index);
//}
//
//void ERFilterSortProxy::remove(int base_index)
//{
//	auto it = std::find(m_map_to_base.begin(), m_map_to_base.end(), base_index);
//	if (it == m_map_to_base.end()) return;
//	int index = it - m_map_to_base.begin();
//	m_map_to_base.erase(it);
//	emit sDelete(index);
//}

void ERFilterSortProxy::clearInternal()
{
	if (!m_base) return;
	//emit sBeginReset();
	// disconnect everything
	for (size_t i = 0; i < m_base->size(); i++) {
		EResource *res = m_base->child(i);
		disconnect(this, 0, res, 0);
	}
	m_map_to_base.clear();
	//emit sReset();
}

void ERFilterSortProxy::reload()
{
	if (!m_base) return;
	emit sAboutToReset();
	clearInternal();

	std::vector<size_t> nodes_filtered;
	for (size_t i = 0; i < m_base->size(); i++) {
		EResource *node = m_base->child(i);
		// track(node);
		if (accept(node)) {
			nodes_filtered.push_back(i);
		}
	}

	auto lt = [this](size_t l, size_t r) { return lessThan(l, r); };
	std::sort(nodes_filtered.begin(), nodes_filtered.end(), lt);

	m_map_to_base = nodes_filtered;

	qDebug() << "emit reset" << m_map_to_base.size();
	emit sReset();
}

void ERFilterSortProxy::updateCategorySet(int model_row)
{
	if (model_row == 0) return;
	ECategory *cat = TGroupModel<ECategory>::get(m_category_filters, model_row);
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

void ERFilterSortProxy::removeAndSignal(const std::vector<size_t>& removals)
{
	QList<size_t> s;
	for (auto i : removals) s.append(i);
	emit sAboutToRemoveMulti(removals);
	VecUtil::multiRemove(&m_map_to_base, removals);
	emit sRemovedMulti(removals);
}

int ERFilterSortProxy::indexOf(const EResource *node) const
{
	if (!m_base) return -1;
	for (int i = 0; i < (int)size(); i++) {
		if (child(i) == node) return i;
	}
	return -1;
}

//void ERFilterSortProxy::track(EResource *res)
//{
//
//	// listen to the new item
//	//connect(res, &EResource::sResourceNameChanged, this, [this, res]() { onResourceChange(res); });
//	//connect(res, &EResource::sCategoryChanged, this, [this, res]() { onResourceChange(res); });
//	//connect(res, &EResource::sGlobalChanged, this, [this, res]() { onResourceChange(res); });
//}
