#ifndef ERFILTERSORTPROXY_H
#define ERFILTERSORTPROXY_H

#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <osg/Vec3>

#include "Core/GroupTemplate.h"
#include "EREnums.h"

class EResource;
class ECategory;
class ECategoryGroup;
class CheckableListProxy;

class ERFilterSortProxy : public TGroup<EResource> {
	Q_OBJECT
public:
	ERFilterSortProxy(TGroup<EResource> *base = nullptr);

	void setBase(TGroup<EResource> *base);

	// group overrides
	EResource *child(size_t index) const override;
	size_t size() const override;
	int indexOf(const EResource *node) const override;

	// same thing
	std::vector<size_t> mapFromBase(std::vector<size_t> base_indices) const;
	std::set<size_t> baseToLocal(const std::set<size_t> base_indices) const;
	//std::vector<size_t> indicesOf(EResource *res) const;

	// not supported
	//virtual bool insertChild(unsigned int index, Node *child) override;
	//virtual bool removeChildren(unsigned int index, unsigned int numChildrenToRemove) override;

	//typedef bool(*LessThanFunc)(osg::Node*, osg::Node*);
	//static bool lessThanNone(osg::Node *left, osg::Node *right);
	//static bool lessThanDistance(osg::Node *left, osg::Node *right);
	//static bool lessThanAlphabetical(osg::Node *left, osg::Node *right);
	virtual bool lessThan(size_t left, size_t right);

	// information

	// get list of indices for resources
	// if blanks is true then missing resources have index -1
	std::vector<int> indicesOf(const std::vector<EResource*> &res, bool blanks = true);

	// filter sort overrides
	virtual bool accept(EResource *node);

	//virtual LessThanFunc lessThanFunc() const;
	//virtual void track(EResource *node);

	EResource *getResource(int index) const;
	void debug();

	// configuration
	//void setCategories(ECategoryGroup *categories);

	void sortBy(ER::SortBy method);
	//SortBy getSortMethod() const;

	void setCategories(CheckableListProxy *cats);
	void setFiletypes(CheckableListProxy *types);

	void showLocal(bool local);
	void showGlobal(bool global);
	void enableRange(bool enable);
	void enableYears(bool enable); // both have to be true for time test
	void appTimeEnable(bool enable); // this is for the global
	//void setSearchRadius(float radius);

	void setSearch(const QString &search);
	void setYear(int year);
	//void setPosition(osg::Vec3 pos);

	// resorts stuff based on distance
	void positionChangePoke();

	// redoes everything, emits reset signals
	void reload();
	// redoes everything, emits remove/insert/move signals
	void reload2();

signals:
	void sSortByChanged(ER::SortBy);
	void sRadiusChanged(float radius);
	void sUseRangeChanged(bool use);
	void sUseYearsChanged(bool use);
	void sShowLocalChanged(bool show);
	void sShowGlobalChanged(bool show);
	void sSearchChanged(const QString &s);
	//void sTextSearchChanged(const std::string &text);
	//void sFileTypesChanged();

protected:
	// add/removes a resource if it is acceptable
	//void checkAndAdd(int base_index);

	//
	//void checkAndAddSet(std::set<int> base_index);


	void recheck(const std::set<size_t> &base_indices);
	void checkAndInsertSet(const std::set<size_t> &base_indices);
	void checkAndRemoveSet(const std::set<size_t> &base_indices);

	// when a name changes or something
	void onResourceChange(EResource *res);

	// rechecks all resources
	//void rescan();
	//void add(int base_index);
	//void remove(int base_index);

	void clearInternal();
	//void remap(int base_index);
	//bool inMap(EResource *res);



private:
	// adds/remove from m_categories_enabled based on model
	void updateCategorySet(int model_row);
	bool checkSearch(const EResource *res, const QString &s) const;

private:
	// filter sort proxy
	TGroup<EResource> *m_base;
	std::vector<size_t> m_map_to_base;

	// ER specific
	ER::SortBy m_sort_by;

	CheckableListProxy *m_category_filters;
	std::set<const ECategory*> m_categories_enabled;
	CheckableListProxy *m_type_filters;
	std::set<std::string> m_types_enabled;

	bool m_show_global;
	bool m_show_local;
	bool m_enable_range;
	bool m_enable_years;
	bool m_time_enabled;

	QString m_search;
	int m_year;
	
	bool m_show_all;

	//osg::Vec3 m_position;
	//double m_radius;
	//std::unordered_set<EResource*> m_in_range;
};

#endif