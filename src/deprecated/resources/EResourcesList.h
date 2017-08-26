/*
 * EResourcesList.cpp
 *
 *  Created on: December 28, 2011
 *      Author: Franklin Fang
 */

#ifndef ERESOURCES_LIST_H_
#define ERESOURCES_LIST_H_

#include "resources/EResourcesNode.h"
#include "resources/EResourcesCategory.h"
#include "osgNewWidget/VSWidget.h"

typedef std::vector<osg::ref_ptr<EResourcesNode> > EResourcesNodeVector;
typedef std::vector<osg::ref_ptr<VSLabel> > WidgetVector;

class EResourcesList:public osg::Node{

public:
	explicit EResourcesList();
	virtual ~EResourcesList();

    EResourcesList(const EResourcesList& e, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
        : osg::Node(e, copyop), m_selection(e.m_selection), m_list(e.m_list), m_widgets(e.m_widgets) {}

    META_Node(, EResourcesList)

	EResourcesNode* GetSelectedNode(){
		assert(m_selection >= 0);
		return m_list[m_selection];
	}
	void AddList(EResourcesList* e){m_list.insert(m_list.end(),e->m_list.begin(),e->m_list.end());
	m_widgets.insert(m_widgets.end(),e->m_widgets.begin(),e->m_widgets.end());}
	int getSize(){assert(m_list.size()==m_widgets.size()); return m_list.size();}
	bool getLock() const{return m_locked;}
	void setLock(bool lock){ m_locked = lock;}	
	int GetSelect(){ return m_selection; }
	int GetOver(){return m_over;}
    void SetSelect(int index) { m_selection = index; }
	void SetOver(int index){ m_over = index;}
	void Clear(){m_list.clear();m_widgets.clear();m_selection = -1; m_over = -1;}
	osg::ref_ptr<EResourcesNode> updateNode( std::string title, std::string filetype, std::string filepath, std::string filediscription, std::string authors, int copyright, erCategory category, int global, 
		int minyear, int maxyear, float localrange, int reposition, int launch, int ertype);
	osg::ref_ptr<EResourcesNode> addNode(std::string title, std::string filetype, std::string filepath, 
		std::string filediscription, std::string authors, int copyright, erCategory category, int global, int minyear, int maxyear, float localrange, int reposition, int launch, int ertype);
	void removeNode();
private:
    int m_selection;//the id of selected node
	int m_over;//the id of mouse over node
	bool m_locked;

public:
    EResourcesNodeVector m_list;//the vector that stores the EResourcesNode objects
    WidgetVector m_widgets;//the vector that stores the VSLabel objects
};

typedef std::vector<osg::ref_ptr<EResourcesList> > EResourcesListVector;


#endif