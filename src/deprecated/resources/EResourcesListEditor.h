/*
 * EResourcesListEditor.cpp
 *
 *  Created on: December 28, 2011
 *      Author: Franklin Fang
 */

#ifndef ERESOURCES_LIST_EDITOR_H_
#define ERESOURCES_LIST_EDITOR_H_

//#include "AnimVar.h"
//#include "overlay/OverlayManager.h"
//#include "ObjectManipulator.h"

#include "osgNewWidget/VSWidget.h"	
#include "resources/EResourcesList.h"
#include "resources/EResourcesCategory.h"
#include "resources/EResourcesCategoryList.h"
#include "resources/EResourcesTitleList.h"
#include "resources/EResourcesTypeList.h"
#include "resources/EResourcesFilter.h"
;

class EResourcesTransitioner;
class EResourcesListEditor: public osgGA::GUIEventHandler,
	public osgNewWidget::IWidgetEventHandler, public osgNewWidget::IWidgetAnimationCallback{

public:
	EResourcesListEditor();
	virtual ~EResourcesListEditor();
	void init();
	void exportEResources();
	void importEResources();
	void importEResources(const std::string& filename);
	void addEResourcesToNode(osg::Node* node);
	void extractEResourcesFromNode(osg::Node* node);
	void addEResourcesFromNode(osg::Node* node);
    void drawMultiLeft();
	void drawMultiRight();
	
	void showBar();
	void hideBar();
	void showhide();
	void showBar2();
	void hideBar2();
	bool IsBarOpen();
	float getBarHeight();
	void updateFilteredLists();
	void updateRangedList();
	void updateEndLists();//update the other lists from master list

private:
	void initWidgets();
	void setWidgets();
	virtual void animationEnded(osgNewWidget::Widget* widget);
	void handle(osgNewWidget::Widget* widget, const osgNewWidget::WidgetEvent& ev);
	void handleMouseMove(osgNewWidget::Widget* widget);
	void handleMouseScroll(osgNewWidget::Widget* widget);
	void handleMouseLeftClick(osgNewWidget::Widget* widget);
	void handleMouseDoubleClick(osgNewWidget::Widget* widget);
	bool SelectResource(osgNewWidget::Widget* widget);//check if a resource widget is selected
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
        osg::Object* object, osg::NodeVisitor* nodeVisitor);
	void resetAnchors();
	osg::Vec2f getPosition();
	void addButtonLeft();
	void addButtonRight();
	void editButton();
	void removeButton();
	void searchInputButton();
	void UnselectAll();
	//void updateCentralList(int list_id); //update the master list
	void highlightButton(int button_type);
	void unhighlightButton(int button_type);
	void unhighlightAll();
	void copyList(osg::ref_ptr<EResourcesList> toList, osg::ref_ptr<EResourcesList> fromList);
	void showEResources(bool launch);
	void saveEResources(std::string filename);
	void loadEResources(std::string filename);
	void extractLocalGlobal(osg::ref_ptr<EResourcesList> toList, osg::ref_ptr<EResourcesList> fromList, int local_global);
	//void updateFromLocalGlobalRight();
	//void updateFromLocalGlobalLeft();
	void processCommand(wxString& command);
	void ajustBarRatioButton();
	void UpdateSearch();
	//void testListInit();//for test purpose only
	void setHoldGravity(bool value);


protected:

	int m_status; 
	int m_action; 
	int m_list_over;//which file list(both left and right) have cursor moving over
	int m_list_select;//which file list(both left and right) have resource selected
	int m_file_list_selection;//which file list is selected to display on the right bar
    float m_bar_ratio;
	osg::Vec2f m_leftbar_local_offset;
	osg::Vec2f m_leftbar_global_offset;
	osg::Vec2f m_rightbar_category_offset;
	osg::Vec2f m_rightbar_type_offset;
	osg::Vec2f m_rightbar_title_offset;
	osg::Vec2f m_rightbar_global_offset;
	osg::Vec2f m_rightbar_local_offset;
	osg::Vec2f m_rightbar_search_offset;

	EResourcesTransitioner* m_transitioner;
	float m_last_frame_time;

	osg::ref_ptr<VSCanvas> m_widget_canvas;
	
	osg::ref_ptr<VSLabel> m_widget_hide_left;
	osg::ref_ptr<VSLabel> m_widget_hide_right;
	osg::ref_ptr<VSLabel> m_widget_left_local_label;
	osg::ref_ptr<VSLabel> m_widget_left_global_label;
	osg::ref_ptr<VSLabel> m_widget_search_label;
	osg::ref_ptr<VSLabel> m_widget_search_input;
	osg::ref_ptr<VSLabel> m_widget_search_cancel;
    osg::ref_ptr<VSLabel> m_widget_add_left;
    osg::ref_ptr<VSLabel> m_widget_remove_left;
	osg::ref_ptr<VSLabel> m_widget_edit_left;
	osg::ref_ptr<VSLabel> m_widget_add_right;
    osg::ref_ptr<VSLabel> m_widget_remove_right;
	osg::ref_ptr<VSLabel> m_widget_edit_right;
	osg::ref_ptr<VSLabel> m_widget_right_2_left;
	osg::ref_ptr<VSLabel> m_widget_list;
	osg::ref_ptr<VSLabel> m_widget_filter;
	osg::ref_ptr<VSLabel> m_widget_filter_cancel;
	osg::ref_ptr<VSLabel> m_widget_title;
	osg::ref_ptr<VSLabel> m_widget_category;
	osg::ref_ptr<VSLabel> m_widget_filetype;
	osg::ref_ptr<VSLabel> m_widget_global;
	osg::ref_ptr<VSLabel> m_widget_local;
	osg::ref_ptr<VSLabel> m_widget_bar_ratio;
	osg::ref_ptr<VSLabel> m_widget_locked_left;
	osg::ref_ptr<VSLabel> m_widget_locked_right;

	osg::ref_ptr<VSMulti> m_widget_multi_local;
	osg::ref_ptr<VSMulti> m_widget_multi_global;
	osg::ref_ptr<VSMulti> m_widget_multi_list;

	osg::ref_ptr<EResourcesList> m_eresources_central_list; //central/master list
	osg::ref_ptr<EResourcesList> m_eresources_ranged_list; //part of central list, selected based on min/max time
	//comparision with the time slider

	//end lists
	osg::ref_ptr<EResourcesList> m_eresources_local_list_left; 
	osg::ref_ptr<EResourcesList> m_eresources_global_list_left;
	//the reason for left and right lists are due to difference of the widgets.
	osg::ref_ptr<EResourcesList> m_eresources_local_list_right;
	osg::ref_ptr<EResourcesList> m_eresources_global_list_right;
	osg::ref_ptr<EResourcesList> m_eresources_filted_list;
	osg::ref_ptr<EResourcesList> m_eresources_searched_list;


public:
	EResourcesCategoryList m_category_list;
	EResourcesTitleList m_title_list;
	EResourcesTypeList m_type_list;
	EResourcesFilter m_filter;
};



class EResourcesTransitioner{
public:
	EResourcesTransitioner(){
		m_node = NULL;
		m_on_off = false;
		m_timer = 0.0f;
		m_transition_time = 1.0f;//transition time
	};
	virtual ~EResourcesTransitioner(){};
	void TurnOn(osg::ref_ptr<EResourcesNode> node){
		m_on_off = true;
		SetTransitionNode(node);
	}
	void TurnOff(){m_on_off = false; ResetTimer(); m_node = 0;}
	void ResetTimer(){m_timer = 0.0f;}
	void SetTimer(float t){m_timer = t;}
	float ReadTimer(){return m_timer;}
	void SetTransitionNode(osg::ref_ptr<EResourcesNode> node);
	void SetTransitionTime(float t){
		m_transition_time = t;}
	void ApplyTransition(float dt);
	bool IsOn(){
		return m_on_off;
	}

private: 
	osg::ref_ptr<EResourcesNode> m_node;
	float m_timer;
	float m_transition_time;
	bool m_on_off;//true on, false off;
	osg::Matrixd m_matrix_from;
	osg::Matrixd m_matrix_to;

};

#endif