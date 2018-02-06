#ifndef ERFILTERSORTPROXY_H
#define ERFILTERSORTPROXY_H

#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

#include "Group.h"

class EResource;
class ECategory;
class ECategoryGroup;
class CheckableListProxy;

class ERFilterSortProxy : public Group {
	Q_OBJECT
public:
	ERFilterSortProxy(Group *base);

	void setBase(Group *base);

	// group overrides
	virtual osg::Node *child(unsigned int index) const override;
	virtual unsigned int getNumChildren() const override;
	virtual int indexOf(const osg::Node *node) const override;
	// not supported
	//virtual bool insertChild(unsigned int index, Node *child) override;
	//virtual bool removeChildren(unsigned int index, unsigned int numChildrenToRemove) override;

	// filter sort overrides
	virtual bool accept(osg::Node *node);
	virtual bool lessThan(int left, int right);
	virtual void track(osg::Node *node);

	EResource *getResource(int index) const;
	void debug();

	// configuration
	//void setCategories(ECategoryGroup *categories);

	enum SortBy {
		ALPHABETICAL,
		DISTANCE,
		NONE,
	};
	void sortBy(SortBy method);
	//SortBy getSortMethod() const;

	void setCategories(CheckableListProxy *cats);
	void setFiletypes(CheckableListProxy *types);

	void showLocal(bool local);
	void showGlobal(bool global);
	void enableRange(bool enable);
	void enableYears(bool enable);

	void setTitleSearch(const std::string &title);

	void setPosition(osg::Vec3f pos);


protected:
	// add/removes a resource if it is acceptable
	void checkAndAdd(int base_index);

	//
	void checkAndAddSet(std::set<int> base_index);

	// when a name changes or something
	void onResourceChange(EResource *res);

	// rechecks all resources
	void rescan();
	void add(int base_index);
	void remove(int base_index);
	void clear();
	//void remap(int base_index);
	//bool inMap(EResource *res);

private:
	void updateCategorySet(int model_row);
	bool checkTitle(const std::string &s) const;

private:
	// filter sort proxy
	osg::ref_ptr<Group> m_base;
	std::vector<int> m_map_to_base;

	// ER specific
	SortBy m_sort_by;

	CheckableListProxy *m_category_filters;
	std::set<const ECategory*> m_categories_enabled;
	CheckableListProxy *m_type_filters;
	std::set<std::string> m_types_enabled;

	bool m_show_global;
	bool m_show_local;
	bool m_enable_range;
	bool m_enable_years;

	std::string m_title_search;
	
	bool m_show_all;

	osg::Vec3f m_position;
};

#endif