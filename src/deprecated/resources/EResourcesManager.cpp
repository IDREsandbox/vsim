/*
 * EResourcesManager.cpp
 *
 *  Created on: December 28, 2011
 *      Author: Franklin Fang
 */
#include "test_Prefix.pch"
#include "wx/utils.h"
#include "resources/EResourcesManager.h"
#include "resources/EResourcesListEditor.h"
#include "NRTSViewer.h"

EResourcesManager::EResourcesManager(){
	
}


EResourcesManager::~EResourcesManager(){
	delete m_eresources_list_editor;
}

void EResourcesManager::init(){
	
	m_eresources_list_editor = new EResourcesListEditor;
	m_eresources_list_editor->init();
	g_viewer->addEventHandler(m_eresources_list_editor);

}

void EResourcesManager::importEResources(const std::string& filename){
	m_eresources_list_editor->importEResources(filename);
}

void EResourcesManager::importEResources(){
	m_eresources_list_editor->importEResources();
}

void EResourcesManager::updateTimeRange(){
	//TODO check if mouse is up?if not do not update, if up update
	if(!wxGetMouseState().LeftIsDown())
		m_eresources_list_editor->updateEndLists();
}

void EResourcesManager::exportEResources(){
	m_eresources_list_editor->exportEResources();
}

void EResourcesManager::addEResourcesToNode(osg::Node* node){
	m_eresources_list_editor->addEResourcesToNode(node);
}

void EResourcesManager::extractEResourcesFromNode(osg::Node* node){
		
	m_eresources_list_editor->extractEResourcesFromNode(node);
}

void EResourcesManager::addEResourcesFromNode(osg::Node* node){
		
	m_eresources_list_editor->addEResourcesFromNode(node);
}

void EResourcesManager::showBar()
{
	m_eresources_list_editor->showBar();
}

void EResourcesManager::hideBar()
{
	m_eresources_list_editor->hideBar();
}

void EResourcesManager::showBar2()
{
	m_eresources_list_editor->showBar2();
	
}

void EResourcesManager::hideBar2()
{
	m_eresources_list_editor->hideBar2();
	
}

void EResourcesManager::showhide()
{
	m_eresources_list_editor->showhide();
	
}

bool EResourcesManager::isBarOpen(){return m_eresources_list_editor->IsBarOpen();}

float EResourcesManager::BarHeight(){
	return m_eresources_list_editor->getBarHeight();
}