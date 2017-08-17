/*
 * EResourcesNode.cpp
 *
 *  Created on: December 15, 2011
 *      Author: Franklin Fang
 */
#include "test_Prefix.pch"
#include "resources/EResourcesNode.h"

EResourcesNode::EResourcesNode(){

	m_name = "default";
	m_filetype = "default";
	m_filepath = "";
	m_description = "default";
	m_global = 1;//if false, then local
	m_reposition = 1;//if true,auto-on
	m_launch = 1;//if true, auto-on
	m_local_range = 10;//global should be false to make this valid
	m_ertype = 0;
	m_min_year = -9999;
	m_max_year = 9999;
}

EResourcesNode::~EResourcesNode(){

}

EResourcesNode &EResourcesNode:: operator= (const EResourcesNode & copy){
	
	m_name = copy.m_name;
    m_filetype = copy.m_filetype;
	m_filepath = copy.m_filepath;
	m_description = copy.m_description;
	m_global = copy.m_global;
	m_reposition = copy.m_reposition;
	m_launch = copy.m_launch;
	m_local_range = copy.m_local_range;//global should be false to make this valid
	m_ertype = copy.m_ertype;
	m_category = copy.m_category;
	m_min_year = copy.m_min_year;
	m_max_year = copy.m_max_year;

	return *this;//allow assignments to be chained
}

bool EResourcesNode::operator== (const EResourcesNode &other){
	
	if(m_name != other.m_name)
		return false;
    if(m_filetype != other.m_filetype)
		return false;
	if(m_filepath != other.m_filepath)
		return false;
	if(m_description != other.m_description)
		return false;
	if(m_global != other.m_global)
		return false;
	if(m_reposition != other.m_reposition)
		return false;
	if(m_launch != other.m_launch)
		return false;
	if(m_local_range != other.m_local_range)
		return false;//global should be false to make this valid
	if(m_ertype != other.m_ertype)
		return false;
	if(m_category != other.m_category)
		return false;
	if(m_min_year != other.m_min_year)
		return false;
	if(m_max_year != other.m_max_year)
		return false;

	return true;
}
