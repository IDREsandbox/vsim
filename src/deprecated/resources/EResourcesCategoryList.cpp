/*
 * EResourcesCategoryList.cpp
 *
 *  Created on: January 4, 2012
 *      Author: Franklin Fang
 */

#include "test_Prefix.pch"
#include "resources/EResourcesCategoryList.h"
#include "resources/EResourcesNode.h"
#include "resources/EResourcesCategory.h"
#include "ui/NewCategoryDialog.h"

EResourcesCategoryList::EResourcesCategoryList(){
	m_selection = -1;
	m_over = -1;
	m_expand = false;
}

EResourcesCategoryList::~EResourcesCategoryList(){

}

void EResourcesCategoryList::NewCategory(){
	NewCategoryDialog *dlg = new NewCategoryDialog("Create New Category");
	if(!(dlg->show())){
		return;
	}

	//add the category node
	erCategory newcat;
	newcat.Name = dlg->name_txt->GetLineText(0);
	newcat.Red = dlg->category_color->GetColour().Red();
	newcat.Green = dlg->category_color->GetColour().Green();
	newcat.Blue = dlg->category_color->GetColour().Blue();
    m_category_vector.push_back(newcat);
	
	//add the widget node
	VSLabel* new_category_label = new VSLabel(newcat.Name);
	new_category_label->setSize(osg::Vec2f(90.0f, 90.0f));
	new_category_label->setPosition(osg::Vec2f(0.0f, 0.0f));
	//set color
	float Red = newcat.Red/255.0;
	float Green = newcat.Green/255.0;
	float Blue = newcat.Blue/255.0;
	new_category_label->setColor(osg::Vec4f(Red,Green,Blue,0.3f));
	m_widgets.push_back(new_category_label);
	
	//add the list
	osg::ref_ptr<EResourcesList> newlist = new EResourcesList;
	newlist->SetSelect(-1);
	newlist->SetOver(-1);
	m_eresources_list_vector.push_back(newlist);
}

void EResourcesCategoryList::AddCategory(erCategory category){
    m_category_vector.push_back(category);
}

void EResourcesCategoryList::RemoveCategory(int list_id){

	//need to remove the whole list
	if(list_id >= 0){
		m_category_vector.erase(m_category_vector.begin() + list_id);
		m_widgets.erase(m_widgets.begin() + list_id);
		m_eresources_list_vector.erase(m_eresources_list_vector.begin()+list_id);
	}

}	
void EResourcesCategoryList::EditCategory(){

	if(m_selection == -1)
		return;
	NewCategoryDialog *dlg = new NewCategoryDialog("Edit Catergory");
	dlg->name_txt->SetValue(m_category_vector[m_selection].Name);
	dlg->category_color->SetColour(wxColour(m_category_vector[m_selection].Red,m_category_vector[m_selection].Green,m_category_vector[m_selection].Blue));
	dlg->show();
	erCategory newcat;
	newcat.Name = dlg->name_txt->GetLineText(0);
	newcat.Red = dlg->category_color->GetColour().Red();
	newcat.Green = dlg->category_color->GetColour().Green();
	newcat.Blue = dlg->category_color->GetColour().Blue();
	
	UpdateCategory(newcat);//update the list for the selected category 
}	

void EResourcesCategoryList::UpdateCategory(erCategory newcat){
	if(m_selection == -1)
		return;
	m_category_vector[m_selection] = newcat;
	m_widgets[m_selection]->setColor(osg::Vec4f(newcat.Red/255.0,newcat.Green/255.0,newcat.Blue/255.0,0.3f));

	size_t iter;
	for(iter = 0; iter < m_eresources_list_vector[m_selection]->m_list.size(); iter++)
	{
		m_eresources_list_vector[m_selection]->m_list[iter]->m_category = newcat;	
	}
}

void EResourcesCategoryList::UpdateContent(osg::ref_ptr<EResourcesList> eresources_list){

	m_category_vector.clear();
	m_widgets.clear();
	m_eresources_list_vector.clear();
	EResourcesNode* currentNode;
	VSLabel* currentWidget;
	int l_list = eresources_list->m_list.size();
	int category_index;

	for(int iter = 0; iter < l_list; iter++) {
		currentNode = eresources_list->m_list[iter];//point to the old one
		currentWidget = new VSLabel(eresources_list->m_widgets[iter]->getLabel());//create a new one
		//first determine category
		category_index = LocateCategory(currentNode->m_category);
		//if find one, then insert into the list
		if(category_index >= 0)
		{
			m_eresources_list_vector[category_index]->m_list.push_back(currentNode);
			m_eresources_list_vector[category_index]->m_widgets.push_back(currentWidget);
		}
		//else, create the category, then create the list, inset into the list
		else{//category_index = -1;
			m_category_vector.push_back(currentNode->m_category);
			
			VSLabel* new_category_label = new VSLabel(currentNode->m_category.Name);
			new_category_label->setSize(osg::Vec2f(90.0f, 90.0f));
			new_category_label->setPosition(osg::Vec2f(0.0f, 0.0f));
			//set color
			float Red = currentNode->m_category.Red/255.0;
			float Green = currentNode->m_category.Green/255.0;
			float Blue = currentNode->m_category.Blue/255.0;
			new_category_label->setColor(osg::Vec4f(Red,Green,Blue,0.3f));
			m_widgets.push_back(new_category_label);

			osg::ref_ptr<EResourcesList> newlist = new EResourcesList;
			newlist->m_list.push_back(currentNode);
			newlist->m_widgets.push_back(currentWidget);
			m_eresources_list_vector.push_back(newlist);
		}
	}

}


void EResourcesCategoryList::OutputContent(osg::ref_ptr<EResourcesList> eresources_list){
	
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

int EResourcesCategoryList::LocateCategory(erCategory input_category){
	int l_category = m_category_vector.size();
	for(int iter = 0; iter < l_category; iter++){
		if(m_category_vector[iter].Name == input_category.Name)
			return iter;
	}
	return -1;
}

EResourcesNode* EResourcesCategoryList::GetSelectedNode(){

	if(m_over < 0)
		return NULL;
	if(m_selection < 0)
		return NULL;
	return m_eresources_list_vector[m_over]->m_list[m_selection];

}

erCategory& EResourcesCategoryList::GetSelectedCategory(){
	assert(m_selection >= 0);
	return m_category_vector[m_selection];
}