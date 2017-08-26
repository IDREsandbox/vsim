/*
 * EResourcesManager.h
 *
 *  Created on: December 28, 2011
 *      Author: Franklin Fang
 */

#ifndef ERESOURCESMANAGER_H_
#define ERESOURCESMANAGER_H_

class EResourcesListEditor;

class EResourcesManager
{
public:
	EResourcesManager();
	virtual ~EResourcesManager();
	void init();
	void importEResources();
	void updateTimeRange();
	void importEResources(const std::string& filename);
    void exportEResources();
	void addEResourcesToNode(osg::Node* node);
	void extractEResourcesFromNode(osg::Node* node);
	void addEResourcesFromNode(osg::Node* node);
    void showBar();
    void hideBar();
	void showBar2();
    void hideBar2();
	void showhide();
	bool isBarOpen();
	float BarHeight();

protected:
	osg::ref_ptr<EResourcesListEditor> m_eresources_list_editor;
};

#endif
