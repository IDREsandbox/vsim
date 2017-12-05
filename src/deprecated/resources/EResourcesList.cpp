/*
 * EResourcesList.cpp
 *
 *  Created on: December 28, 2011
 *      Author: Franklin Fang
 */

#include "deprecated/resources/EResourcesNode.h"
#include "deprecated/resources/EResourcesList.h"
#include <QDebug>
EResourcesList::EResourcesList():m_selection(-1),m_over(-1),m_locked(false){
	
}

EResourcesList::~EResourcesList(){
	
}

osg::ref_ptr<EResourcesNode> EResourcesList::addNode(std::string title, std::string filetype, 
							 std::string filepath, std::string filediscription, std::string authors, int copyright, erCategory category, 
							 int global, int minyear, int maxyear, float localrange, int reposition, int launch, int ertype){	
	
	osg::ref_ptr<EResourcesNode> newnode;//add a new node into the vector of nodes
	newnode = new EResourcesNode;
	newnode->m_name = title;
	newnode->m_filetype = filetype;
	newnode->m_filepath = filepath;
	newnode->m_authors = authors;
	newnode->m_copyright = copyright;
	newnode->m_category = category;
	newnode->m_description = filediscription;
	newnode->m_global = global;
	newnode->m_min_year = minyear;
	newnode->m_max_year = maxyear;
	newnode->m_local_range = localrange;
	newnode->m_reposition = reposition;
	newnode->m_launch = launch;
	newnode->m_ertype = ertype;

	//***newnode->setViewMatrix(g_viewer->getCameraManipulator()->getMatrix());
	
	osg::ref_ptr<VSLabel> newlabel;
	newlabel = new VSLabel(filetype);//the name of the node is temporarily set to the filetype
	
	m_list.push_back(newnode);
	m_widgets.push_back(newlabel);

	return newnode;

}

osg::ref_ptr<EResourcesNode> EResourcesList::updateNode(std::string title, std::string filetype, 
								std::string filepath, std::string filediscription, std::string authors, int copyright, erCategory category,
								int global, int minyear, int maxyear, float localrange, int reposition, int launch, int ertype){

	int index = m_selection;

	m_list[index]->m_name = title;
	m_list[index]->m_filepath = filepath;
	m_list[index]->m_filetype = filetype;
	m_list[index]->m_authors = authors;
	m_list[index]->m_category = category;
	m_list[index]->m_copyright = copyright; 
	m_list[index]->m_description = filediscription;
	m_list[index]->m_global = global;
	m_list[index]->m_min_year = minyear;
	m_list[index]->m_max_year = maxyear;
	m_list[index]->m_local_range = localrange;
	m_list[index]->m_reposition = reposition;
	m_list[index]->m_launch = launch;
	m_list[index]->m_ertype = ertype;

	m_widgets[index]->setLabel(filetype);

	//ask if want to reset the view matrix;
	//MessageDialog md;
	//bool result = md.show("Reset the Desired View for the Resource?", MESSAGE_DIALOG_YES_NO);
	//if(result)
	//	m_list[index]->setViewMatrix(g_viewer->getCameraManipulator()->getMatrix());

	return m_list[index];
}

void EResourcesList::removeNode(){

	int index = m_selection;
	
	if(index >= 0 && index < (signed int) m_list.size()){
		EResourcesNodeVector::iterator er_to_delete;
		er_to_delete = m_list.begin()+index;
		m_list.erase(er_to_delete);

		WidgetVector::iterator w_to_delete;
		w_to_delete = m_widgets.begin()+index;
		m_widgets.erase(w_to_delete);
	}

	m_selection = -1;
}

