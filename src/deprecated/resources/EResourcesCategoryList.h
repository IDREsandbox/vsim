/*
 * EResourcesCategoryList.h
 *
 *  Created on: January 4, 2012
 *      Author: Franklin Fang
 */

#ifndef ERESOURCES_CATEGORY_LIST_H_
#define ERESOURCES_CATEGORY_LIST_H_

#include "resources/EResourcesCategory.h"
#include "resources/EResourcesList.h"
#include "osgNewWidget/VSWidget.h"

typedef std::vector<osg::ref_ptr<VSLabel> > WidgetVector;

class EResourcesCategoryList{
	
public:
	EResourcesCategoryList();
	virtual ~EResourcesCategoryList();
	EResourcesNode* GetSelectedNode();
	erCategory& GetSelectedCategory();
	int GetSelect(){return m_selection;}
	void SetSelect(int index){m_selection = index;}
	int GetOver(){return m_over;}
	void SetOver(int index){m_over = index;}
	void NewCategory();
	void AddCategory(erCategory category);
	CategoryVector* GetCategoryVector(){return &m_category_vector;}
	void RemoveCategory(int list_id);
	void EditCategory();
	void UpdateCategory(erCategory newcat);
	int LocateCategory(erCategory input_category);
	void UpdateContent(osg::ref_ptr<EResourcesList> eresources_list);
	void OutputContent(osg::ref_ptr<EResourcesList> eresources_list);
	bool m_expand;

	CategoryVector m_category_vector;
	WidgetVector m_widgets;
	EResourcesListVector m_eresources_list_vector;//list of pointers to EResourcesList

private:
	int m_selection;//which category node is selected
	int m_over;
};

#endif

