/*
 * EResourcesTitleList.h
 *
 *  Created on: January 4, 2012
 *      Author: Franklin Fang
 */

#ifndef ERESOURCES_TYPE_LIST_H_
#define ERESOURCES_TYPE_LIST_H_

#include "resources/EResourcesCategory.h"
#include "resources/EResourcesList.h"
#include "osgNewWidget/VSWidget.h"

typedef std::vector<osg::ref_ptr<VSLabel> > WidgetVector;
typedef std::vector<std::string> FileTypeVector;//store "pdf", "doc" ..., "png"
class EResourcesTypeList{
	
public:
	EResourcesTypeList();
	virtual ~EResourcesTypeList();
	EResourcesNode* GetSelectedNode();
	std::string& GetSelectedCategory();
	int GetSelect(){return m_selection;}
	void SetSelect(int index){m_selection = index;}
	int GetOver(){return m_over;}
	void SetOver(int index){m_over = index;}
	void UpdateContent(osg::ref_ptr<EResourcesList> eresources_list);
	void OutputContent(osg::ref_ptr<EResourcesList> eresources_list);
	int LocateType(std::string name);
	char GetInitial(std::string name);

	bool m_expand;

	FileTypeVector m_type_vector;
	WidgetVector m_widgets;
	EResourcesListVector m_eresources_list_vector;//list of pointers to EResourcesList

private:
	int m_selection;//which category node is selected
	int m_over;

};

#endif
