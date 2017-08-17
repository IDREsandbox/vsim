/*
 * EResourcesList.cpp
 *
 *  Created on: 9/27/2012
 *      Author: Franklin Fang
 */

#include "test_Prefix.pch"
#include "resources/EResourcesFilter.h"
#include "resources/EResourcesList.h"
#include "resources/EResourcesListEditor.h"
#include "resources/EResourcesCategory.h"
#include "resources/EResourcesCategoryList.h"
#include "resources/EResourcesTitleList.h"
#include "resources/EResourcesTypeList.h"
#include "menu/Settings.h"

EResourcesFilter::EResourcesFilter(){
	m_filter_type_count = 0;
	m_filter_title_count = 0;
	m_filter_category_count = 0;
}

EResourcesFilter::~EResourcesFilter(){

}

void EResourcesFilter::init(osg::ref_ptr<VSCanvas> a_root_canvas, osg::ref_ptr<EResourcesListEditor> a_ere_list_editor){
	//create the widgets
	assert(a_root_canvas);
	assert(a_ere_list_editor);
	m_root_canvas = a_root_canvas;
	m_ere_list_editor = a_ere_list_editor;

	m_label_title = new VSLabel("By Title");
	m_label_title->setBorderColor(osg::Vec4f(1,1,1,0));
	m_label_title->setColor(osg::Vec4f(0,0,0,1));
	m_label_title->setHandler(this);
	m_root_canvas->addWidget(m_label_title.get());

	m_label_category = new VSLabel("By Category");
	m_label_category->setBorderColor(osg::Vec4f(1,1,1,0));
	m_label_category->setColor(osg::Vec4f(0,0,0,1));
	m_label_category->setHandler(this);
	m_root_canvas->addWidget(m_label_category.get());

	m_label_filetype = new VSLabel("By FileType");
	m_label_filetype->setBorderColor(osg::Vec4f(1,1,1,0));
	m_label_filetype->setColor(osg::Vec4f(0,0,0,1));
	m_label_filetype->setHandler(this);
	m_root_canvas->addWidget(m_label_filetype.get());

	//set size and position
	m_label_title->setSize(osg::Vec2f(110, 30));
	m_label_category->setSize(osg::Vec2f(110, 30));
	m_label_filetype->setSize(osg::Vec2f(110, 30));

}

void EResourcesFilter::setWidgets(){

	//reset widgets, previous marks are delted. 
	//This function should be called only during init or when there is an update in the 
	//central list such that current filter tags might be affected.

	//set the counters back to 0
	m_filter_type_count = 0;
	m_filter_title_count = 0;
	m_filter_category_count = 0;

	//set the position of label widgets
	//remove all the widgets
	for(unsigned int i = 0; i< m_tags_vector_title.size(); i++)
		m_root_canvas->removeWidget(m_tags_vector_title[i]->m_widget);
	for(unsigned int i = 0; i< m_tags_vector_category.size(); i++)
		m_root_canvas->removeWidget(m_tags_vector_category[i]->m_widget);
	for(unsigned int i = 0; i< m_tags_vector_filetype.size(); i++)
		m_root_canvas->removeWidget(m_tags_vector_filetype[i]->m_widget);

	m_tags_vector_title.clear();
	m_tags_vector_category.clear();
	m_tags_vector_filetype.clear();
	FilterTag* temp_tag;

	//create the tag widgets
	for(unsigned int i = 0; i < m_ere_list_editor->m_title_list.m_title_vector.size(); i++)
	{
			temp_tag = new FilterTag;
			std::string title;
			title = m_ere_list_editor->m_title_list.m_title_vector[i];
			temp_tag->m_widget = new VSLabel(title);
			m_tags_vector_title.push_back(temp_tag);
			temp_tag->m_widget->setSize(osg::Vec2f(30,30));
			temp_tag->m_widget->setBorderColor(osg::Vec4f(1,1,1,0));
			temp_tag->m_widget->setColor(osg::Vec4f(0.7, 0.2, 0.2, 0.5));
			temp_tag->m_widget->setHandler(this);
			m_root_canvas->addWidget(temp_tag->m_widget.get());
	}
	
	for(unsigned int i = 0; i < m_ere_list_editor->m_category_list.m_category_vector.size(); i++)
	{
			temp_tag = new FilterTag;
			std::string name;
			name = m_ere_list_editor->m_category_list.m_category_vector[i].Name;
			temp_tag->m_widget = new VSLabel(name);
			m_tags_vector_category.push_back(temp_tag);
			temp_tag->m_widget->setSize(osg::Vec2f(100,30));
			temp_tag->m_widget->setBorderColor(osg::Vec4f(1,1,1,0));
			temp_tag->m_widget->setColor(osg::Vec4f(0.7, 0.2, 0.2, 0.5));
			temp_tag->m_widget->setHandler(this);
			m_root_canvas->addWidget(temp_tag->m_widget.get());
	}

	for(unsigned int i = 0; i < m_ere_list_editor->m_type_list.m_type_vector.size(); i++)
	{
			temp_tag = new FilterTag;
			std::string type;
			type = m_ere_list_editor->m_type_list.m_type_vector[i];
			temp_tag->m_widget = new VSLabel(type);
			m_tags_vector_filetype.push_back(temp_tag);
			temp_tag->m_widget->setSize(osg::Vec2f(65,30));
			temp_tag->m_widget->setBorderColor(osg::Vec4f(1,1,1,0));
			temp_tag->m_widget->setColor(osg::Vec4f(0.7, 0.2, 0.2, 0.5));
			temp_tag->m_widget->setHandler(this);
			m_root_canvas->addWidget(temp_tag->m_widget.get());
	}
	
	setWidgetPositions();
	
}	

void EResourcesFilter::setWidgetPositions(){
	//also will change root canvas size in y direction
	
	osgNewWidget::Canvas* uiCanvas = osgNewWidget::WidgetManager::instance()->getUIRoot();
	osg::Vec2f rsize = uiCanvas->getSize();

	float x_position, y_position;
	x_position = 110;	y_position = 30;
	//calibrate the position of labels
	for(unsigned int i = 0; i < m_tags_vector_category.size(); i++)
	{
			x_position += 105;
			if(x_position + 100 > rsize.x()){
				x_position = 215;
				y_position += 35;
			}
	}
	
	x_position = 145; y_position += 35;
	for(unsigned int i = 0; i < m_tags_vector_filetype.size(); i++)
	{
			x_position += 70;
			if(x_position + 65 > rsize.x()){
				x_position = 215;
				y_position += 35;
			}
	}

	x_position = 180; y_position += 35;
	for(unsigned int i = 0; i < m_tags_vector_title.size(); i++)
	{
			x_position += 35;
			if(x_position + 35 > rsize.x()){
				x_position = 215;
				y_position += 35;
			}

	}
	
	y_position += 30;

	m_root_canvas->setSize(osg::Vec2f(rsize.x()*2, y_position + g_settings.m_bar_size_2));

	x_position = 110;	y_position = y_position - 30;

	m_label_category->setPosition(osg::Vec2f(26, y_position));

	for(unsigned int i = 0; i < m_tags_vector_category.size(); i++)
	{

			x_position += 105;
			if(x_position + 100 > rsize.x()){
				x_position = 215;
				y_position -= 35;
			}
			m_tags_vector_category[i]->m_widget->setPosition(osg::Vec2f(x_position, y_position));
	}
	
	x_position = 145; y_position -= 35;
	m_label_filetype->setPosition(osg::Vec2f(26, y_position));
	
	for(unsigned int i = 0; i < m_tags_vector_filetype.size(); i++)
	{
			x_position += 70;
			if(x_position + 65 > rsize.x()){
				x_position = 215;
				y_position -= 35;
			}
			m_tags_vector_filetype[i]->m_widget->setPosition(osg::Vec2f(x_position, y_position));
	}

	x_position = 180; y_position -= 35;
	m_label_title->setPosition(osg::Vec2f(26, y_position));

	for(unsigned int i = 0; i < m_tags_vector_title.size(); i++)
	{
			x_position += 35;
			if(x_position + 35 > rsize.x()){
				x_position = 215;
				y_position -= 35;
			}
			m_tags_vector_title[i]->m_widget->setPosition(osg::Vec2f(x_position, y_position));
	}

}

void EResourcesFilter::applyFilter(osg::ref_ptr<EResourcesList> original_list, osg::ref_ptr<EResourcesList> target_list){
	
	//empty current list;
	target_list->m_list.clear();
	target_list->m_widgets.clear();

	//there is at least one filter on in this situation

	osg::ref_ptr<EResourcesList> temp_list_1, temp_list_2;
	temp_list_1 = new EResourcesList();
	temp_list_2 = new EResourcesList();

	temp_list_1->m_list.clear();
	temp_list_1->m_widgets.clear();
	temp_list_2->m_list.clear();
	temp_list_2->m_widgets.clear();


	//filter out all the selected titled resources, put in a temporary list

	if(m_filter_title_count == 0)//no filter applied
	{
		//copy the whole list to temp list 1
		for(unsigned int i = 0; i < original_list->m_list.size(); i++){
			temp_list_1->m_list.push_back(original_list->m_list[i]);//shallow copy
			temp_list_1->m_widgets.push_back(new VSLabel(original_list->m_widgets[i]->getLabel()));//deep copy
		}
	
	}
	else
	{

		for(unsigned int j = 0; j < m_tags_vector_title.size(); j++){
			if(m_tags_vector_title[j]->m_select){
				//need to filter all the resources with this title
				std::string tag_title = m_tags_vector_title[j]->m_widget->getLabel();
				std::string current_initial;
				for(unsigned int i = 0; i < original_list->m_list.size(); i++){
					current_initial = GetInitial(original_list->m_list[i]->m_name);
					if(current_initial == tag_title)
					{
						temp_list_1->m_list.push_back(original_list->m_list[i]);//shallow copy
						temp_list_1->m_widgets.push_back(new VSLabel(original_list->m_widgets[i]->getLabel()));//deep copy
					}
				}//end for
			}//end if
		}//end for
	}

	//filter out all the selected category resources, put in a temporary list

	if(m_filter_category_count == 0)//no filter applied
	{
		//copy the whole temp list 1 to temp list 2
		for(unsigned int i = 0; i < temp_list_1->m_list.size(); i++){
			temp_list_2->m_list.push_back(temp_list_1->m_list[i]);//shallow copy
			temp_list_2->m_widgets.push_back(new VSLabel(temp_list_1->m_widgets[i]->getLabel()));//deep copy
		}
	
	}
	else
	{
		for(unsigned int j = 0; j < m_tags_vector_category.size(); j++){
			if(m_tags_vector_category[j]->m_select){
				//need to filter all the resources with this title
				std::string tag_category = m_tags_vector_category[j]->m_widget->getLabel();
				for(unsigned int i = 0; i < temp_list_1->m_list.size(); i++){
					if(temp_list_1->m_list[i]->m_category.Name == tag_category)
					{
						temp_list_2->m_list.push_back(temp_list_1->m_list[i]);//shallow copy
						temp_list_2->m_widgets.push_back(new VSLabel(temp_list_1->m_widgets[i]->getLabel()));//deep copy
					}
				}//end for
			}//end if
		}//end for
	}

	//filter out all the selected type resoruces, put in the target list

	if(m_filter_type_count == 0)//no filter applied
	{
		//copy the whole temp list 1 to temp list 2
		for(unsigned int i = 0; i < temp_list_2->m_list.size(); i++){
			target_list->m_list.push_back(temp_list_2->m_list[i]);//shallow copy
			target_list->m_widgets.push_back(new VSLabel(temp_list_2->m_widgets[i]->getLabel()));//deep copy
		}
	
	}
	else
	{
		for(unsigned int j = 0; j < m_tags_vector_filetype.size(); j++){
			if(m_tags_vector_filetype[j]->m_select){
				//need to filter all the resources with this title
				std::string cmp_label = m_tags_vector_filetype[j]->m_widget->getLabel();
				for(unsigned int i = 0; i < temp_list_2->m_list.size(); i++){
					if(temp_list_2->m_widgets[i]->getLabel() == cmp_label)
					{
						target_list->m_list.push_back(temp_list_2->m_list[i]);//shallow copy
						target_list->m_widgets.push_back(new VSLabel(temp_list_2->m_widgets[i]->getLabel()));//deep copy
					}
				}//end for
			}//end if
		}//end for
	}

}

void EResourcesFilter::handle(osgNewWidget::Widget* widget, const osgNewWidget::WidgetEvent& ev){
	
	switch(ev.type){

	case osgNewWidget::WidgetEvent::MOUSE_MOVE:
	
		handleMouseMove(widget);

		break;

	case osgNewWidget::WidgetEvent::MOUSE_PUSH:   

		handleMouseLeftClick(widget);
		
		break;

	default:
		return;
	}//end switch

}


void EResourcesFilter::handleMouseMove(osgNewWidget::Widget* widget){

	std::string show_text;
	bool show_delay = false;
	osgNewWidget::InfoBoxPosition show_position = osgNewWidget::DOWN_RIGHT;


	if(widget == m_label_title)
	{
		show_text = "Filter by title <A - Z>";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_label_category)
	{
		show_text = "Filter by different category";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_label_filetype)
	{
		show_text = "Filter by different file type";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	
	show_text = "Left-Click to apply/cancel filter";
	osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
	return;
	

}

void EResourcesFilter::handleMouseLeftClick(osgNewWidget::Widget* widget){

	for(unsigned int i = 0; i < m_tags_vector_title.size(); i++)
	{
		if(widget == m_tags_vector_title[i]->m_widget)
		{
			//register or cancel filter
			m_tags_vector_title[i]->m_select = m_tags_vector_title[i]->m_select ? false:true;
			m_filter_title_count += m_tags_vector_title[i]->m_select ? 1:-1;
			//change color of the tag
			m_tags_vector_title[i]->m_widget->setColor(m_tags_vector_title[i]->m_select ? osg::Vec4f(0,0.5,0, 0.5) : osg::Vec4f(0.7, 0.2, 0.2, 0.5));

			m_ere_list_editor->updateFilteredLists();

			return;			
		}
	}

	for(unsigned int i = 0; i < m_tags_vector_filetype.size(); i++)
	{
		if(widget == m_tags_vector_filetype[i]->m_widget)
		{
			//register or cancel filter
			m_tags_vector_filetype[i]->m_select = m_tags_vector_filetype[i]->m_select ? false:true;
			m_filter_type_count += m_tags_vector_filetype[i]->m_select ? 1:-1;
			//change color of the tag
			m_tags_vector_filetype[i]->m_widget->setColor(m_tags_vector_filetype[i]->m_select ? osg::Vec4f(0,0.5,0,0.5) : osg::Vec4f(0.7, 0.2, 0.2, 0.5));
			
			m_ere_list_editor->updateFilteredLists();

			return;
		}
	}

	for(unsigned int i = 0; i < m_tags_vector_category.size(); i++)
	{
		if(widget == m_tags_vector_category[i]->m_widget)
		{
			//register or cancel filter
			m_tags_vector_category[i]->m_select = m_tags_vector_category[i]->m_select ? false:true;
			m_filter_category_count += m_tags_vector_category[i]->m_select ? 1:-1;
			m_tags_vector_category[i]->m_widget->setColor(m_tags_vector_category[i]->m_select ? osg::Vec4f(0,0.5,0, 0.5) : osg::Vec4f(0.7, 0.2, 0.2, 0.5));
			//change color of the tag

			m_ere_list_editor->updateFilteredLists();

			return;
		}
	}


}	

char EResourcesFilter::GetInitial(std::string name){
	char l_initial = name[0];
	if (l_initial >= 97 && l_initial <= 122)
		return l_initial-32;//a -> A
	else
		return l_initial;//everything else
}


bool EResourcesFilter::IsFilterOn(){
	
	if(m_filter_title_count + m_filter_type_count + m_filter_category_count == 0)
		return false;

	return true;

}