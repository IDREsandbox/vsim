/*
 * EResourcesTypeList.cpp
 *
 *  Created on: January 4, 2012
 *      Author: Franklin Fang
 */

#include "test_Prefix.pch"
#include "resources/EResourcesTypeList.h"

EResourcesTypeList::EResourcesTypeList(){
	m_selection = -1;
	m_over = -1;
	m_expand = false;
}

EResourcesTypeList::~EResourcesTypeList(){

}


void EResourcesTypeList::UpdateContent(osg::ref_ptr<EResourcesList> eresources_list){

	//extract from the central list

	m_type_vector.clear();
	m_widgets.clear();
	m_eresources_list_vector.clear();

	EResourcesNode* currentNode;
	VSLabel* currentWidget;
	int l_list = eresources_list->m_list.size();
	int type_index;

	for(int iter = 0; iter < l_list; iter++) {
		currentNode = eresources_list->m_list[iter];//point to the old one
		currentWidget = new VSLabel(eresources_list->m_widgets[iter]->getLabel());//create a new one
		type_index = LocateType(currentNode->m_filetype);//return -1 if not found
		//if find one, then insert into the list
		if(type_index >= 0)
		{
			m_eresources_list_vector[type_index]->m_list.push_back(currentNode);
			m_eresources_list_vector[type_index]->m_widgets.push_back(currentWidget);
		}
		//else, create the category, then create the list, inset into the list
		else{//not found, create a new list
			VSLabel* new_type_label = new VSLabel(currentNode->m_filetype);
			new_type_label->setSize(osg::Vec2f(90.0f, 90.0f));
			new_type_label->setPosition(osg::Vec2f(0.0f, 0.0f));

			osg::ref_ptr<EResourcesList> newlist = new EResourcesList;
			newlist->SetSelect(-1);
			newlist->SetOver(-1);
			newlist->m_list.push_back(currentNode);
			newlist->m_widgets.push_back(currentWidget);

			m_type_vector.push_back(currentNode->m_filetype);
			m_widgets.push_back(new_type_label);	
			m_eresources_list_vector.push_back(newlist);
		}
	}

}



void EResourcesTypeList::OutputContent(osg::ref_ptr<EResourcesList> eresources_list){

	//parse the list vector and fill up the global and local list seperately
	eresources_list->m_list.clear();
	eresources_list->m_widgets.clear();

	int num_list = m_eresources_list_vector.size();
	int num_node;
	VSLabel *templabel;
	VSLabel *newlabel;
	EResourcesNode * tempnode;
	for(int iter1 = 0; iter1 < num_list; iter1++){
		num_node = m_eresources_list_vector[iter1]->m_list.size();
		for(int iter2 = 0; iter2 < num_node; iter2++){
			tempnode = m_eresources_list_vector[iter1]->m_list[iter2];
			templabel = m_eresources_list_vector[iter1]->m_widgets[iter2];

			eresources_list->m_list.push_back(tempnode);
			newlabel = new VSLabel(templabel->getLabel());
			eresources_list->m_widgets.push_back(newlabel);
		}
	
	}


}
int EResourcesTypeList::LocateType(std::string type){

	for(size_t iter = 0; iter < m_type_vector.size(); iter++){
		if(m_type_vector[iter] == type)
			return iter;
	}

	return -1;
}

EResourcesNode* EResourcesTypeList::GetSelectedNode(){

	if(m_over < 0)
		return NULL;
	if(m_selection < 0)
		return NULL;
	return m_eresources_list_vector[m_over]->m_list[m_selection];

}

std::string& EResourcesTypeList::GetSelectedCategory(){
	assert(m_over < 0);

	return m_type_vector[m_over];
}