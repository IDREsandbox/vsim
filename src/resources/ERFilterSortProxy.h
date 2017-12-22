#ifndef ERFILTERSORTPROXY_H
#define ERFILTERSORTPROXY_H

#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>
#include "resources/ECategory.h"
#include "resources/EResource.h"

class ERFilterSortProxy : public Group {
	Q_OBJECT
public:
	ERFilterSortProxy(Group *base);

	enum SortBy {
		ALPHABETICAL,
		DISTANCE,
		NONE,
	};
	void sortBy(SortBy method);
	//SortBy getSortMethod() const;

	void addCategory(ECategory *cat);
	void removeCategory(ECategory *cat);

	void addFiletype(const std::string& extension);
	void removeFiletype(const std::string& extension);

	void addTitleSearch(const std::string& extension);
	void removeTitleSearch(const std::string& extension);

	enum FilterGlobal {
		SHOW_BOTH,
		SHOW_GLOBAL,
		SHOW_LOCAL
	};
	void filterGlobal(FilterGlobal what);

	void enableRange(bool enable);

	void setPosition(osg::Vec3f pos);

	// overrides
	virtual void setBase(Group *base);
	virtual osg::Node *child(unsigned int index) const override;
	virtual unsigned int getNumChildren() const override;
	virtual int indexOf(const osg::Node *node) const override;

	EResource *getResource(int index) const;
	void debug();

	//set position

	// not supported
	//virtual bool insertChild(unsigned int index, Node *child) override;
	//virtual bool removeChildren(unsigned int index, unsigned int numChildrenToRemove) override;

	// does the filter accept this category?
	bool accept(EResource *res);

private:
	// add/removes a resource if it is acceptable
	void checkAndAdd(int base_index);

	// when a name changes or something
	void onResourceChange(EResource *res);

	// rechecks all resources
	void rescan();
	void add(int base_index);
	void remove(int base_index);
	void clear();
	//void remap(int base_index);
	//bool inMap(EResource *res);

	void track(int base_index);

private:
	osg::ref_ptr<Group> m_base;

	SortBy m_sort_by;
	std::set<ECategory*> m_enable_categories;
	std::set<std::string> m_enable_filetypes;
	std::set<std::string> m_name_filters;
	bool m_enable_all;
	FilterGlobal m_filter_global;
	osg::Vec3f m_position;

	//std::map<std::string, int> m_title_map;
	//std::map<float, int> m_distance_map;

	std::vector<int> m_map_to_base;

	// for ALPHABETICAL
	//std::map<std::string, EResource*> m_title_map;
	//std::unordered_map<EResource*, std::map<std::string, EResource*>::iterator> m_title_hashmap;

	// for NONE
	//std::vector<EResource*> m_included;
	//std::unordered_map<EResource*, std::set<EResource*>::iterator> m_included_map;
};

#endif