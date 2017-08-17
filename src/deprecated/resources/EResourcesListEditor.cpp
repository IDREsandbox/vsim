/*
 * EResourcesListEditorListEditor.cpp
 *
 *  Created on: December 28, 2011
 *      Author: Franklin Fang
 */

//#include "overlay/OverlayManager.h"
//#include "ObjectManipulator.h"

#include "test_Prefix.pch"
#include "util.h"
#include "NRTSViewer.h"
#include "AnimVar.h"
#include "NRTSBaseManipulator.h"
#include "NRTSManipulator.h"
#include "USIMManipulator.h"
#include "NRTSKeySwitchMatrixManipulator.h"
#include "VSimApp.h"
#include "osgNewWidget/WidgetManager.h"
#include "resources/EResourcesListEditor.h"
#include "resources/EResourcesList.h"
#include "resources/EResourcesNode.h"
#include "resources/EResourcesFilter.h"
#include "resources/EResourcesInfoCard.h"
#include "ui/NewEResourceDialog.h"
#include "ui/MessageDialog.h"
#include "ui/ERExportOptionDialog.h"
#include "ui/GenericFileDialog.h"
#include "ui/SearchDialog.h"
#include "ui/AdjustRatioDialog.h"
#include "ui/ERLaunchDialog.h"
#include "menu/Settings.h"
#include "menu/TimeSwitch.h"
#include "overlay/BrandingManager.h"
#include "ui/ERExportOptionConfirmation.h"
#include "wxOsgApp.h"

#include <cstring>



#define FILE_SAVE_ASCII

//bar status
#define WIDGET_CLOSED_LEFT 1
#define WIDGET_CLOSED_RIGHT 2
#define WIDGET_HALF_LEFT 3
#define WIDGET_HALF_RIGHT 4
#define WIDGET_FULL_LEFT 5

//bar action
#define WIDGET_CLOSED2HALF 1
#define WIDGET_HALF2CLOSED 2
#define WIDGET_LEFT2RIGHT 3
#define WIDGET_RIGHT2LEFT 4
#define WIDGET_HALF2FULL 5
#define WIDGET_FULL2HALF 6
#define WIDGET_NOACTION -1

//lists
#define LIST_GLOBAL_RIGHT 1
#define LIST_LOCAL_RIGHT 2
#define LIST_TITLE 3
#define LIST_CATEGORY 4
#define LIST_TYPE 5
#define LIST_GLOBAL_LEFT 6
#define LIST_LOCAL_LEFT 7
#define LIST_SEARCH 8
#define LIST_NOSELECTION -1

#define BUTTON_LIST 1
#define BUTTON_FILTER 2
#define BUTTON_TITLE 3
#define BUTTON_CATEGORY 4
#define BUTTON_TYPE 5
#define BUTTON_ADD 6
#define BUTTON_EDIT 7
#define BUTTON_DELETE 8
#define BUTTON_GLOBAL 9
#define BUTTON_LOCAL 10
#define BUTTON_ADD_2 11
#define BUTTON_DELETE_2 12
#define BUTTON_EDIT_2 13
#define BUTTON_SEARCH_INPUT 14
#define BUTTON_SEARCH_LABEL 15
#define BUTTON_SEARCH_CANCEL 16
#define BUTTON_NOSELECTION -1

wxDECLARE_APP(wxOsgApp);

bool cmpCategory(osg::ref_ptr<EResourcesNode> node);
bool cmpWidget(osg::ref_ptr<VSLabel> node);

EResourcesListEditor::EResourcesListEditor(){
	m_status = WIDGET_HALF_LEFT;
	m_action = WIDGET_NOACTION;
	m_list_over = LIST_LOCAL_LEFT;
	m_last_frame_time = g_viewer->getFrameStamp()->getReferenceTime();;
};

EResourcesListEditor::~EResourcesListEditor(){

}

void EResourcesListEditor::init(){
	
	m_bar_ratio = 0.6;
	m_action = WIDGET_NOACTION;
	m_list_over = LIST_NOSELECTION;
	m_list_over = LIST_NOSELECTION;
	m_status = WIDGET_HALF_LEFT;
	m_file_list_selection = LIST_NOSELECTION;

	m_leftbar_local_offset = osg::Vec2f(0.0f,0.0f);
	m_leftbar_global_offset = osg::Vec2f(0.0f,0.0f);
	m_rightbar_category_offset = osg::Vec2f(0.0f,0.0f);
	m_rightbar_type_offset = osg::Vec2f(0.0f,0.0f);
	m_rightbar_title_offset = osg::Vec2f(0.0f,0.0f);
	m_rightbar_global_offset = osg::Vec2f(0.0f,0.0f);
	m_rightbar_local_offset = osg::Vec2f(0.0f,0.0f);
	m_rightbar_search_offset = osg::Vec2f(0.0f,0.0f);

	m_transitioner = new EResourcesTransitioner();
	m_eresources_ranged_list = new EResourcesList();
    m_eresources_central_list = new EResourcesList();
	m_eresources_local_list_left = new EResourcesList();
	m_eresources_global_list_left = new EResourcesList();
	m_eresources_local_list_right = new EResourcesList();
	m_eresources_global_list_right = new EResourcesList();
	m_eresources_filted_list = new EResourcesList();
	m_eresources_searched_list = new EResourcesList();

	//for test purpose only
	//testListInit();//generate a ere list in memory

	initWidgets(); //init the widget objects and position these
	setWidgets(); //widgets are positioned smartly, by looking at the window_size and lists (local/global ere numbers)
	
	updateEndLists(); //create end lists from the master list

	m_file_list_selection = LIST_CATEGORY;
	highlightButton(BUTTON_CATEGORY);
}

void EResourcesListEditor::initWidgets(){

	osgNewWidget::Canvas* uiCanvas = osgNewWidget::WidgetManager::instance()->getUIRoot();
	osg::Vec2f rsize = uiCanvas->getSize();
	
	m_widget_canvas = new VSCanvas(osgNewWidget::WIDGET_WITH_GEOMETRY);
	uiCanvas->addWidget(m_widget_canvas);//add the canvas widget to the root canvas

    m_widget_hide_left = new VSLabel("v");
    m_widget_hide_left->setHandler(this);
	m_widget_hide_left->setZOffset(m_widget_hide_left->getZOffset() + 1);
	uiCanvas->addWidget(m_widget_hide_left);//add the hide widget to the root canvas

	float l_bar_size_ratio = 0.6;// ration/(1 - ratio) = local/global
	float left_x_length = (rsize.x() - 240)*l_bar_size_ratio;
	float right_x_length = (rsize.x() - 240)*(1 - l_bar_size_ratio);

    m_widget_multi_local = new VSMulti(osg::Vec2f(left_x_length, g_settings.m_bar_size_2 - 60.0f));
    m_widget_multi_local->setPosition(osg::Vec2f(180.0f, g_settings.m_bar_size_2 + 20.0f));
    m_widget_canvas->addWidget(m_widget_multi_local.get());//add the multi widget to the canvas widget

	m_widget_left_local_label = new VSLabel("Local Resources (spatially aware) :");
	m_widget_left_local_label->setBorderColor(osg::Vec4f(1,1,1,0));
	m_widget_left_local_label->setColor(osg::Vec4f(1,1,1,0));
	m_widget_canvas->addWidget(m_widget_left_local_label.get());

	m_widget_multi_global = new VSMulti(osg::Vec2f(right_x_length, g_settings.m_bar_size_2 - 60.0f));
    m_widget_multi_global->setPosition(osg::Vec2f(210.0f + left_x_length, g_settings.m_bar_size_2 + 20.0f));
    m_widget_canvas->addWidget(m_widget_multi_global.get());//add the multi widget to the canvas widget

	m_widget_left_global_label = new VSLabel("Global Resources :");
	m_widget_left_global_label->setBorderColor(osg::Vec4f(1,1,1,0));
	m_widget_left_global_label->setColor(osg::Vec4f(1,1,1,0));
	m_widget_canvas->addWidget(m_widget_left_global_label.get());

    m_widget_add_left = new VSLabel("+");
    m_widget_add_left->setHandler(this);
    m_widget_canvas->addWidget(m_widget_add_left.get());//add the play widget to the canvas widget

    m_widget_remove_left = new VSLabel("-");
    m_widget_remove_left->setHandler(this);
    m_widget_canvas->addWidget(m_widget_remove_left.get());//add the pause widget to the canvas widget
	
	m_widget_edit_left = new VSLabel("Edit");
    m_widget_edit_left->setHandler(this);
    m_widget_canvas->addWidget(m_widget_edit_left.get());

	m_widget_list = new VSLabel("Show All");
    m_widget_list->setHandler(this);
    m_widget_canvas->addWidget(m_widget_list.get());

	m_widget_filter = new VSLabel("Filter");
    m_widget_filter->setHandler(this);
	m_widget_filter->setColor(osg::Vec4f(0.7, 0.2, 0.2, 0.5));
    m_widget_canvas->addWidget(m_widget_filter.get());
   

	m_widget_filter_cancel = new VSLabel("X");
    m_widget_filter_cancel->setHandler(this);
    m_widget_canvas->addWidget(m_widget_filter_cancel.get());
  

	m_widget_bar_ratio = new VSLabel("");
	m_widget_bar_ratio->setHandler(this);
	m_widget_bar_ratio->setBorderColor(osg::Vec4f(1,1,1,0));
	m_widget_canvas->addWidget(m_widget_bar_ratio.get());
	//the right side

	m_widget_hide_right = new VSLabel("v");
    m_widget_hide_right->setHandler(this);
	m_widget_hide_right->setZOffset(m_widget_hide_right->getZOffset() + 1);
	uiCanvas->addWidget(m_widget_hide_right);//add the hide widget to the root canvas

	m_widget_multi_list = new VSMulti(osg::Vec2f(rsize.x() - 240.0f, g_settings.m_bar_size_2 - 55.0f));
    m_widget_canvas->addWidget(m_widget_multi_list.get());//add the multi widget to the canvas widget
    m_widget_canvas->setHAnchor(m_widget_multi_list.get(), osgNewWidget::ANCHOR_LR);

	m_widget_right_2_left = new VSLabel("< Back");
    m_widget_right_2_left->setHandler(this);
	m_widget_canvas->addWidget(m_widget_right_2_left.get());//add the hide widget to the root canvas

	m_widget_add_right = new VSLabel("+");
    m_widget_add_right->setHandler(this);
    m_widget_canvas->addWidget(m_widget_add_right.get());//add the play widget to the canvas widget

    m_widget_remove_right = new VSLabel("-");
    m_widget_remove_right->setHandler(this);
    m_widget_canvas->addWidget(m_widget_remove_right.get());//add the pause widget to the canvas widget
	
	m_widget_edit_right = new VSLabel("E");
    m_widget_edit_right->setHandler(this);
    m_widget_canvas->addWidget(m_widget_edit_right.get());

	m_widget_title = new VSLabel("Title");
    m_widget_title->setHandler(this);
    m_widget_canvas->addWidget(m_widget_title.get());

	m_widget_category = new VSLabel("Category");
    m_widget_category->setHandler(this);
    m_widget_canvas->addWidget(m_widget_category.get());

	m_widget_filetype = new VSLabel("FileType");
    m_widget_filetype->setHandler(this);
    m_widget_canvas->addWidget(m_widget_filetype.get());

	m_widget_global = new VSLabel("Global");
    m_widget_global->setHandler(this);
    m_widget_canvas->addWidget(m_widget_global.get());
	
	m_widget_local = new VSLabel("Local");
    m_widget_local->setHandler(this);
    m_widget_canvas->addWidget(m_widget_local.get());

	m_widget_search_label = new VSLabel("Search:");
    m_widget_search_label->setHandler(this);
    m_widget_canvas->addWidget(m_widget_search_label.get());

	m_widget_search_input = new VSLabel("Click to Input for Search");
    m_widget_search_input->setHandler(this);
    m_widget_canvas->addWidget(m_widget_search_input.get());

	m_widget_search_cancel = new VSLabel("X");
    m_widget_search_cancel->setHandler(this);
    m_widget_canvas->addWidget(m_widget_search_cancel.get());

	m_widget_locked_left = new VSLabel("Locked");
	m_widget_locked_left->hide();
    m_widget_canvas->addWidget(m_widget_locked_left.get());

	m_widget_locked_right = new VSLabel("Locked");
	m_widget_locked_right->hide();
    m_widget_canvas->addWidget(m_widget_locked_right.get());


	m_filter.init(m_widget_canvas, this);

}





void EResourcesListEditor::setWidgets(){
//set widgets' position and size based on current windows size, etc.
	osgNewWidget::Canvas* uiCanvas = osgNewWidget::WidgetManager::instance()->getUIRoot();
	osg::Vec2f rsize = uiCanvas->getSize();

	m_filter.setWidgets();

    m_widget_hide_left->setSize(osg::Vec2f(15.0f, 15.0f));
    m_widget_hide_right->setSize(osg::Vec2f(15.0f, 15.0f));

	float canvas_size_y = m_widget_canvas->getSize().y();
	if(m_status == WIDGET_HALF_LEFT)
	{
		m_widget_canvas->setPosition(osg::Vec2f(0.0f, -canvas_size_y + g_settings.m_bar_size_2));
	    m_widget_hide_left->setPosition(osg::Vec2f(10.0f, g_settings.m_bar_size_2-20.0f));
		m_widget_hide_right->setPosition(osg::Vec2f(rsize.x() + 10.0f, g_settings.m_bar_size_2 - 20.0f));

	}
	else if(m_status == WIDGET_HALF_RIGHT)
	{
		m_widget_canvas->setPosition(osg::Vec2f(-rsize.x(), -canvas_size_y + g_settings.m_bar_size_2));
	    m_widget_hide_left->setPosition(osg::Vec2f(-rsize.x() + 10.0f, g_settings.m_bar_size_2-20.0f));
		m_widget_hide_right->setPosition(osg::Vec2f(10.0f, g_settings.m_bar_size_2 - 20.0f));
	}
	else if(m_status == WIDGET_FULL_LEFT)
	{
		m_widget_canvas->setPosition(osg::Vec2f(0.0f, 0.0f));
		m_widget_hide_left->setPosition(osg::Vec2f(10.0f, canvas_size_y - 20.0f));
	    m_widget_hide_right->setPosition(osg::Vec2f(rsize.x() + 10.0f, canvas_size_y - 20.0f));

	}
	else if(m_status == WIDGET_CLOSED_LEFT)
	{
		m_widget_canvas->setPosition(osg::Vec2f(0.0f, -canvas_size_y));
		m_widget_hide_left->setPosition(osg::Vec2f(10.0f, 30.0f));
	    m_widget_hide_right->setPosition(osg::Vec2f(rsize.x() + 10.0f, 30.0f));
	}
	else if(m_status == WIDGET_CLOSED_RIGHT)
	{
		m_widget_canvas->setPosition(osg::Vec2f(-rsize.x(), -canvas_size_y));
		m_widget_hide_left->setPosition(osg::Vec2f(-rsize.x() + 10.0f, 30.0f));
	    m_widget_hide_right->setPosition(osg::Vec2f(10.0f, 30.0f));
	}

	float left_x_length = (rsize.x() - 240)*m_bar_ratio;
	float right_x_length = (rsize.x() - 240)*(1 - m_bar_ratio);

    m_widget_multi_local->setSize(osg::Vec2f(left_x_length, g_settings.m_bar_size_2 - 60.0f));
    m_widget_multi_local->setPosition(osg::Vec2f(180.0f, canvas_size_y - g_settings.m_bar_size_2 + 20.0f));
	m_widget_left_local_label->setSize(osg::Vec2f(300, 26.0f));
	m_widget_left_local_label->setPosition(osg::Vec2f(190.0f, canvas_size_y - 37));
	
	m_widget_bar_ratio->setSize(osg::Vec2f(30.0, g_settings.m_bar_size_2 - 60.0f));
	m_widget_bar_ratio->setPosition(osg::Vec2f(180.0f + left_x_length, canvas_size_y - g_settings.m_bar_size_2 + 20.0f));

	m_widget_multi_global->setSize(osg::Vec2f(right_x_length, g_settings.m_bar_size_2 - 60.0f));
    m_widget_multi_global->setPosition(osg::Vec2f(210.0f + left_x_length, canvas_size_y - g_settings.m_bar_size_2 + 20.0f));
	m_widget_left_global_label->setSize(osg::Vec2f(180, 26.0f));
	m_widget_left_global_label->setPosition(osg::Vec2f(220.0f + left_x_length, canvas_size_y - 37));

	m_widget_add_left->setSize(osg::Vec2f(26.0, 26.0));
    m_widget_add_left->setPosition(osg::Vec2f(25.0f, canvas_size_y - 70.0f));
	m_widget_remove_left->setSize(osg::Vec2f(26.0, 26.0));
    m_widget_remove_left->setPosition(osg::Vec2f(60.0f, canvas_size_y - 70.0f));
	
    m_widget_edit_left->setSize(osg::Vec2f(40.0, 26.0));
    m_widget_edit_left->setPosition(osg::Vec2f(95.0f, canvas_size_y - 70.0f));

    m_widget_list->setSize(osg::Vec2f(110.0, 30.0));
    m_widget_list->setPosition(osg::Vec2f(25.0f, canvas_size_y - 114.0f));

    m_widget_filter->setSize(osg::Vec2f(80.0, 30.0));
    m_widget_filter->setPosition(osg::Vec2f(25.0f,canvas_size_y - 155.0f));

	m_widget_filter_cancel->setSize(osg::Vec2f(30.0, 30.0));
    m_widget_filter_cancel->setPosition(osg::Vec2f(105.0f, canvas_size_y - 155.0f));

	//the right side
	m_widget_multi_list->setSize(osg::Vec2f(rsize.x() - 240.0f, g_settings.m_bar_size_2 - 55.0f));
    m_widget_multi_list->setPosition(osg::Vec2f(rsize.x() + 210.0f,  canvas_size_y - g_settings.m_bar_size_2 + 10.0f));

    m_widget_right_2_left->setSize(osg::Vec2f(85.0f, 35.0f));
    m_widget_right_2_left->setPosition(osg::Vec2f(rsize.x() + 30.0f, canvas_size_y - g_settings.m_bar_size_2 + 65.0f));

	m_widget_add_right->setSize(osg::Vec2f(35.0, 35.0));
    m_widget_add_right->setPosition(osg::Vec2f(rsize.x() + 160.0f, canvas_size_y - 69.0f));

    m_widget_remove_right->setSize(osg::Vec2f(35.0, 35.0));
    m_widget_remove_right->setPosition(osg::Vec2f(rsize.x() + 160.0f,canvas_size_y - 116.0f));

	m_widget_edit_right->setSize(osg::Vec2f(35.0, 35.0));
    m_widget_edit_right->setPosition(osg::Vec2f(rsize.x() + 160.0f, canvas_size_y - 162.0f));

    m_widget_category->setSize(osg::Vec2f(90.0, 26.0));
    m_widget_category->setPosition(osg::Vec2f(rsize.x() + 245.0f, canvas_size_y - 36.0f));

    m_widget_global->setSize(osg::Vec2f(70.0, 26.0));
    m_widget_global->setPosition(osg::Vec2f(rsize.x() + 335.0f, canvas_size_y - 36.0f));

    m_widget_local->setSize(osg::Vec2f(70.0, 26.0));
    m_widget_local->setPosition(osg::Vec2f(rsize.x() + 405.0f, canvas_size_y - 36.0f));

    m_widget_filetype->setSize(osg::Vec2f(90.0, 26.0));
    m_widget_filetype->setPosition(osg::Vec2f(rsize.x() + 475.0f, canvas_size_y- 36.0f));
	
    m_widget_title->setSize(osg::Vec2f(70.0, 26.0));
    m_widget_title->setPosition(osg::Vec2f(rsize.x() + 565.0f, canvas_size_y - 36.0f));

    m_widget_search_label->setSize(osg::Vec2f(60.0, 26.0));
    m_widget_search_label->setPosition(osg::Vec2f(2*rsize.x() -452.0f, canvas_size_y - 36.0f));

    m_widget_search_input->setSize(osg::Vec2f(300.0, 26.0));
    m_widget_search_input->setPosition(osg::Vec2f(2*rsize.x() - 392.0f, canvas_size_y - 36.0f));

    m_widget_search_cancel->setSize(osg::Vec2f(26.0, 26.0));
    m_widget_search_cancel->setPosition(osg::Vec2f(2*rsize.x() - 92.0f, canvas_size_y - 36.0f));

	m_widget_locked_left->setFontColor(osg::Vec4f(1,0,0,0.6));
    m_widget_locked_left->setSize(osg::Vec2f(110.0, 30.0));
	m_widget_locked_right->setFontColor(osg::Vec4f(1,0,0,0.6));
    m_widget_locked_right->setSize(osg::Vec2f(60.0, 35.0));
	m_widget_locked_left->setPosition(osg::Vec2f(25.0f, canvas_size_y - 70.0f));
	m_widget_locked_right->setPosition(osg::Vec2f(rsize.x() + 140.0f, canvas_size_y - 116.0f));

	drawMultiLeft();
	drawMultiRight();

	EResourcesInfoCard::Instance()->Respond2Resize();
}

/*
void EResourcesListEditor::testListInit()
{
	erCategory cat_document("Document", 255,0,0);
	erCategory cat_image("Image",0,255,0 );
	erCategory cat_sound("Sound",0,0,255);
	erCategory cat_video("Video",255,255,0);
	erCategory cat_narrative("Narrative",255,0,255);

	//create the two list;
	m_eresources_central_list->addNode("A Test Title 1", "NOTE", "somepath", "Description for Test File 1 Description for Test File 1\
		Description for Test File 1 Description for Test File 1 Description for Test File 1 Description for Test File 1","Jingyi", 1,
		cat_document, 1, 10, 0, 1,2);
	m_eresources_central_list->addNode("B Test Title 2", "NOTE", "somepath", "Description for Test File 2","Jingyi", 1,
		cat_image, 1, 10, 1, 1,2);
	m_eresources_central_list->addNode("B Test Title 3", "URL", "www.google.com", "Description for Test File 3","Jingyi", 1,
		cat_image, 1, 10, 0, 1,1);
	m_eresources_central_list->addNode("D Test Title 4", "jpg", "somepath", "Description for Test File 4","Jingyi", 1,
		cat_image, 1, 10, 1, 0,0);
	m_eresources_central_list->addNode("Z Test Title 5", "doc", "somepath", "Description for Test File 5","Jingyi", 1,
		cat_document, 0, -1, 0, 1,0);
	m_eresources_central_list->addNode("Z Test Title 6", "docx", "somepath", "Description for Test File 6","Jingyi", 1,
		cat_document, 1, 10, 0, 0,0);
	m_eresources_central_list->addNode("T Test Title 7", "rmvb", "somepath", "Description for Test File 7","Jingyi", 1,
		cat_video, 1, 10, 1, 0,0);
	m_eresources_central_list->addNode("X Test Title 8", "nar", "somepath", "Description for Test File 8","Jingyi", 1,
		cat_narrative, 0, -1, 0, 1,0);
	m_eresources_central_list->addNode("A Test Title 9", "avi", "somepath", "Description for Test File 9","Jingyi", 1,
		cat_video, 0, -1, 1, 0,0);
	m_eresources_central_list->addNode("B Test Title 10", "pptx", "somepath", "Description for Test File 10","Jingyi", 1,
		cat_document, 1, 10, 1, 0,0);
	m_eresources_central_list->addNode("C Test Title 11", "pdf", "somepath", "Description for Test File 11","Jingyi", 1,
		cat_document, 1, 10, 1, 0,0);
	m_eresources_central_list->addNode("M Test Title 12", "wmv", "somepath", "Description for Test File 12","Jingyi", 1,
		cat_sound, 1, 10, 0, 1,0);
	m_eresources_central_list->addNode("B Test Title 14", "mp3", "somepath", "Description for Test File 14","Jingyi", 1,
		cat_sound, 1, 10, 1, 0,0);
	m_eresources_central_list->addNode("T Test Title 15", "nar", "somepath", "Description for Test File 15","Jingyi", 1,
		cat_narrative, 0, -1, 0, 1,0);
	m_eresources_central_list->addNode("X Test Title 16", "mov", "somepath", "Description for Test File 16","Jingyi", 1,
		cat_video, 1, 10, 0, 0,0);
	m_eresources_central_list->addNode("C Test Title 18", "png", "somepath", "Description for Test File 18","Jingyi", 1,
		cat_image, 1, 10, 1, 1,0);
	m_eresources_central_list->addNode("C Test Title 19", "tex", "somepath", "Description for Test File 19","Jingyi", 1,
		cat_document, 1, 10, 0, 0,0);

}
*/



bool EResourcesListEditor::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
    osg::Object* object, osg::NodeVisitor* nodeVisitor)
{
    switch(ea.getEventType())
    {
		case osgGA::GUIEventAdapter::RESIZE:
			setWidgets();//adaptive positioning function
		break;
		case osgGA::GUIEventAdapter::PUSH:
			if(ea.getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON){
				UnselectAll();
				//m_category_list.SetSelect(-1);
				//m_title_list.SetSelect(-1);
				//m_type_list.SetSelect(-1);
			}
		break;
		case osgGA::GUIEventAdapter::FRAME:
			float current_time = g_viewer->getFrameStamp()->getReferenceTime();
			float dt = current_time - m_last_frame_time;
			m_last_frame_time = current_time;
			if(m_transitioner->IsOn())
				m_transitioner->ApplyTransition(dt);
			drawMultiLeft();
		break;

		//case osgGA::GUIEventAdapter::SCROLL:
		//	if (_flags & PROCESS_MOUSE_WHEEL)
		//		return handleMouseWheel(ea, us);
		//	else
		//		return false;
    }	

    return false;
}


void EResourcesListEditor::handle(osgNewWidget::Widget* widget, const osgNewWidget::WidgetEvent& ev)
{
	
	switch(ev.type){

	case osgNewWidget::WidgetEvent::MOUSE_MOVE:
	
		handleMouseMove(widget);

		break;

	case osgNewWidget::WidgetEvent::MOUSE_PUSH:   

		handleMouseLeftClick(widget);
		
		break;

	case osgNewWidget::WidgetEvent::MOUSE_DOUBLE_CLICK:
		
		handleMouseDoubleClick(widget);
	
		break;

	/*case osgNewWidget::WidgetEvent::MOUSE_SCROLL:

		handleMouseScroll(widget);

		break;*/

	default:
		return;
	}//end switch
}

void EResourcesListEditor::handleMouseScroll(osgNewWidget::Widget* widget){
	//void OSGCanvas::OnMouseWheel(wxMouseEvent &event)
	//{
	//	int delta = event.GetWheelRotation() / event.GetWheelDelta() * event.GetLinesPerAction();

	//	if (_graphics_window.valid()) {
	//		_graphics_window->getEventQueue()->mouseScroll(
	//			delta>0 ?
	//			osgGA::GUIEventAdapter::SCROLL_UP :
	//			osgGA::GUIEventAdapter::SCROLL_DOWN);
	//	}
	//}

}

void EResourcesListEditor::handleMouseMove(osgNewWidget::Widget* widget){

	m_eresources_local_list_left->SetOver(-1);
	m_eresources_global_list_left->SetOver(-1);
	m_eresources_global_list_right->SetOver(-1);
	m_eresources_local_list_right->SetOver(-1); //do not set for category, type and tile as -1 as well!
	m_eresources_searched_list->SetOver(-1);

	std::string show_text;
	bool show_delay = false;
	osgNewWidget::InfoBoxPosition show_position = osgNewWidget::DOWN_RIGHT;

	if(widget == m_widget_hide_left)
	{
		show_text = "Left-Click to Hide/Reveal the Embeded Resource Bar";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_hide_right)
	{
		show_text = "Left-Click to Hide/Reveal the Embedded Resource Bar";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_left_local_label)
	{
		show_text = "This bar shows all the surrounding local embedded resources";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_left_global_label)
	{
		show_text = "This bar shows all the global embedded resources";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_search_label)
	{
		show_text = "This bar shows all the search result";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_search_input)
	{
		show_text = "Left-Click to enter texts, search for specific embedded resources";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_search_cancel)
	{
		show_text = "Cancel current search";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}


	if(widget == m_widget_filter_cancel)
	{
		show_text = "Turn off all filter option";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}


	if(widget == m_widget_add_left)
	{
		show_text = "Left-Click to add a new embedded resource";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_add_right)
	{	
		if(m_file_list_selection == LIST_CATEGORY)
			show_text = "Left-Click to add a new embedded resource";
		else{
			show_text = "Left_Click to add a new embedded resource";
		}
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_remove_right)
	{
		if(m_file_list_selection == LIST_CATEGORY)
			show_text = "Left-Click to remove a selected cateogry";
		else if(m_file_list_selection == LIST_TITLE)
			show_text = "No action";
		else if (m_file_list_selection == LIST_TYPE)
			show_text = "No action";
		else
			show_text = "Left-Click to remove a selected embedded resource";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_remove_left)
	{
		show_text = "Left-Click to remove the selected embedded resource";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_edit_left)
	{
		show_text = "Left-Click to edit the selected embedded resource";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_edit_right)
	{
		if(m_file_list_selection == LIST_CATEGORY)
			show_text = "Left-Click to edit a selected cateogry's color and name";
		else if(m_file_list_selection == LIST_TITLE)
			show_text = "No action";
		else if(m_file_list_selection == LIST_TYPE)
			show_text = "No action";
		else
			show_text = "Left-Click to edit the selected embedded resource";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_right_2_left)
	{
		show_text = "Left-Click to return to show embedded resources in current view";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_list)
	{
		show_text = "Left-Click to manage all the embedded resources";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_filter)
	{
		if(m_filter.IsFilterOn())
			show_text = "Filter On, Click to change filters";
		else
			show_text = "Filter Off, Click to select filters";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_title)
	{
		show_text = "Sorted by title";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}
	
	if(widget == m_widget_category)
	{
		show_text = "Sorted by different categories";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_filetype)
	{
		show_text = "Sorted by diffrent file types";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_global)
	{
		show_text = "All global embedded resources";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_local)
	{
		show_text = "All local embedded resources";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}


	if(widget == m_widget_bar_ratio)
	{
		show_text = "Press to change the relative ratio of left and right bar size";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}

	if(widget == m_widget_multi_local||widget == m_widget_multi_global||widget == m_widget_multi_list)
	{
		show_text = "To edit/remove an embedded resource, left click on it";
		osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);
		return;
	}


	show_delay = false;//true means delay forever

	if(m_status == WIDGET_HALF_LEFT || m_status == WIDGET_FULL_LEFT){

		for (size_t i = 0; i < m_eresources_local_list_left->m_widgets.size(); i++)
		{
				if (m_eresources_local_list_left->m_widgets[i].get() == widget)
				{
					 m_list_over = LIST_LOCAL_LEFT;
					 m_eresources_local_list_left->SetOver(i);

					drawMultiLeft();//make the display of current widget larger;

					 return;
				}
		}

		for (size_t i = 0; i < m_eresources_global_list_left->m_widgets.size(); i++)
		{
				if (m_eresources_global_list_left->m_widgets[i].get() == widget)
				{
					m_list_over = LIST_GLOBAL_LEFT;
					m_eresources_global_list_left->SetOver(i);

					drawMultiLeft();

					return;
				}
		}

	}

	if(m_status == WIDGET_HALF_RIGHT)
	{
		switch(m_file_list_selection){
		case LIST_LOCAL_RIGHT:
			for (size_t i = 0; i < m_eresources_local_list_right->m_widgets.size(); i++)
			{
					if (m_eresources_local_list_right->m_widgets[i].get() == widget)
					{
						 m_list_over = LIST_LOCAL_RIGHT;
						 m_eresources_local_list_right->SetOver(i);

						 drawMultiRight();

						 return;
					}
			}
			break;
		case LIST_GLOBAL_RIGHT:
			for (size_t i = 0; i < m_eresources_global_list_right->m_widgets.size(); i++)
			{
				if (m_eresources_global_list_right->m_widgets[i].get() == widget)
				{
					 m_list_over = LIST_GLOBAL_RIGHT;
					 m_eresources_global_list_right->SetOver(i);

					 drawMultiRight();

					 return;
				}
			}
			break;
		case LIST_CATEGORY:

			if(m_category_list.m_expand == false){

				for (size_t i = 0; i < m_category_list.m_category_vector.size(); i++)
				{
					if (m_category_list.m_widgets[i].get() == widget)
					{
						m_list_over = LIST_CATEGORY;
						m_category_list.SetOver(i);

						show_text = "Category <" + m_category_list.m_category_vector[m_category_list.GetOver()].Name
							+ ">, <Click To Select>, <Double Click To Expand>";
						osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);

						drawMultiRight();

						return;
					}
				}
			}
			else{
				size_t list_size = m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_list.size();
				m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->SetOver(-1); 
				for (size_t i = 0; i < list_size; i++){
					if(m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_widgets[i].get() == widget){
						m_list_over = LIST_CATEGORY;
						m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->SetOver(i); 

						drawMultiRight();

						return;
					}
				}
				
			}

			break;
		case LIST_TYPE:
			
			if(m_type_list.m_expand == false){

				for (size_t i = 0; i < m_type_list.m_type_vector.size(); i++)
				{
					if (m_type_list.m_widgets[i].get() == widget)
					{
						m_list_over = LIST_TYPE;
						m_type_list.SetOver(i);

						show_text = std::string("File of type <") + m_type_list.m_type_vector[m_type_list.GetOver()] 
						+ ">, <Click To Select>, <Double Click To Expand>";
						osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);

						drawMultiRight();

						return;
					}
				}
			}
			else{
				size_t list_size = m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_list.size();
				m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->SetOver(-1); 
				for (size_t i = 0; i < list_size; i++){
					if(m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_widgets[i].get() == widget){
						m_list_over = LIST_TYPE;
						m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->SetOver(i); 

						drawMultiRight();

						return;
					}
				}
				
			}
			break;
		case LIST_TITLE:
			if(m_title_list.m_expand == false){

				for (size_t i = 0; i < m_title_list.m_title_vector.size(); i++)
				{
					if (m_title_list.m_widgets[i].get() == widget)
					{
						m_list_over = LIST_TITLE;
						m_title_list.SetOver(i);

						show_text = std::string("Title <") + m_title_list.m_title_vector[m_title_list.GetOver()] 
						+ ">, <Click To Select>, <Double Click To Expand>";
						osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, show_position, show_delay);

						drawMultiRight();

						return;
					}
				}
			}
			else{
				size_t list_size = m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_list.size();
				m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->SetOver(-1); 
				for (size_t i = 0; i < list_size; i++){
					if(m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_widgets[i].get() == widget){
						m_list_over = LIST_TITLE;
						m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->SetOver(i); 

						drawMultiRight();

						return;
					}
				}
				
			}
			break;
		case LIST_SEARCH:
			for (size_t i = 0; i < m_eresources_searched_list->m_widgets.size(); i++)
			{
					if (m_eresources_searched_list->m_widgets[i].get() == widget)
					{
						 m_list_over = LIST_SEARCH;
						 m_eresources_searched_list->SetOver(i);

						 drawMultiRight();

						 return;
					}
			}
			break;
		}
	}


}
void EResourcesListEditor::handleMouseLeftClick(osgNewWidget::Widget* widget){

	if(SelectResource(widget))
		return;//check if one resource is selected in a list;

	if (widget == m_widget_hide_left || widget == m_widget_hide_right )
    {
		EResourcesInfoCard::Instance()->HideInfoCard();
		m_category_list.m_expand = false;
        if (m_status == WIDGET_HALF_LEFT||m_status == WIDGET_HALF_RIGHT)
        {
			m_action = WIDGET_HALF2CLOSED;

            osgNewWidget::WidgetAnimator::instance()->add(m_widget_canvas, osgNewWidget::ANIM_PROPERTY_POS_Y,
                0.0f, 0.33f, -m_widget_canvas->getSize().y(), this);
            osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_left, osgNewWidget::ANIM_PROPERTY_POS_Y,
                0.0f, 0.33f, 30.0f, NULL);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_right, osgNewWidget::ANIM_PROPERTY_POS_Y,
                0.0f, 0.33f, 30.0f, NULL);

			g_vsimapp->getBrandingManager()->LowerMoveDown(g_settings.m_bar_size_2 - 50);
        }
		else if(m_status == WIDGET_CLOSED_LEFT || m_status == WIDGET_CLOSED_RIGHT )
        {
			m_action = WIDGET_CLOSED2HALF;
            osgNewWidget::WidgetAnimator::instance()->add(m_widget_canvas, osgNewWidget::ANIM_PROPERTY_POS_Y,
                0.0f, 0.33f, -m_widget_canvas->getSize().y() + g_settings.m_bar_size_2, this);
            osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_left, osgNewWidget::ANIM_PROPERTY_POS_Y,
                0.0f, 0.33f, g_settings.m_bar_size_2-20.0f, NULL);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_right, osgNewWidget::ANIM_PROPERTY_POS_Y,
                0.0f, 0.33f, g_settings.m_bar_size_2-20.0f, NULL);

			g_vsimapp->getBrandingManager()->LowerMoveUp(g_settings.m_bar_size_2 - 50);
        }
		else if(m_status == WIDGET_FULL_LEFT)
		{
			m_action = WIDGET_FULL2HALF;
            osgNewWidget::WidgetAnimator::instance()->add(m_widget_canvas, osgNewWidget::ANIM_PROPERTY_POS_Y,
                0.0f, 0.33f, -m_widget_canvas->getSize().y() + g_settings.m_bar_size_2, this);
            osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_left, osgNewWidget::ANIM_PROPERTY_POS_Y,
                0.0f, 0.33f, g_settings.m_bar_size_2-20.0f, NULL);

			g_vsimapp->getBrandingManager()->LowerMoveDown(m_widget_canvas->getSize().y() - g_settings.m_bar_size_2);
		}

		return;
    }
	
	if (widget == m_widget_list)
	{
		EResourcesInfoCard::Instance()->HideInfoCard();
        osgNewWidget::Canvas* uiCanvas = osgNewWidget::WidgetManager::instance()->getUIRoot();
        osg::Vec2f rsize = uiCanvas->getSize();
        if (m_status == WIDGET_HALF_LEFT)
        {
			m_action = WIDGET_LEFT2RIGHT;

			drawMultiRight();

            osgNewWidget::WidgetAnimator::instance()->add(m_widget_canvas, osgNewWidget::ANIM_PROPERTY_POS_X,
                0.0f, 0.75f, -rsize.x(), this);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_left, osgNewWidget::ANIM_PROPERTY_POS_X,
                0.0f, 0.75f, -rsize.x() + 10.0f, NULL);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_right, osgNewWidget::ANIM_PROPERTY_POS_X,
                0.0f, 0.75f, 10.0f, NULL);
        }
		else if(m_status == WIDGET_FULL_LEFT){
			m_action = WIDGET_LEFT2RIGHT;
			drawMultiRight();
			
			//full to half
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_canvas, osgNewWidget::ANIM_PROPERTY_POS_Y,
				0.0f, 0.35f, -m_widget_canvas->getSize().y() + g_settings.m_bar_size_2, this);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_left, osgNewWidget::ANIM_PROPERTY_POS_Y,
				0.0f, 0.35f, g_settings.m_bar_size_2-20.0f, NULL);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_right, osgNewWidget::ANIM_PROPERTY_POS_Y,
					0.0f, 0.35f, g_settings.m_bar_size_2-20.0f, NULL);
			//left to right
            osgNewWidget::WidgetAnimator::instance()->add(m_widget_canvas, osgNewWidget::ANIM_PROPERTY_POS_X,
                0.35f, 1.0f, -rsize.x(), this);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_left, osgNewWidget::ANIM_PROPERTY_POS_X,
                0.35f, 1.0f, -rsize.x() + 10.0f, NULL);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_right, osgNewWidget::ANIM_PROPERTY_POS_X,
                0.35f, 1.0f, 10.0f, NULL);
		}
		return;
	}
	
	if (widget == m_widget_right_2_left){
		EResourcesInfoCard::Instance()->HideInfoCard();
		osgNewWidget::Canvas* rootCanvas = osgNewWidget::WidgetManager::instance()->getRoot();
		osg::Vec2f rsize = rootCanvas->getSize();
		m_action = WIDGET_RIGHT2LEFT;
        osgNewWidget::WidgetAnimator::instance()->add(m_widget_canvas, osgNewWidget::ANIM_PROPERTY_POS_X,
            0.0f, 0.75f, 0, this);
        osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_left, osgNewWidget::ANIM_PROPERTY_POS_X,
            0.0f, 0.75f, 10.0f, NULL);
		osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_right, osgNewWidget::ANIM_PROPERTY_POS_X,
                0.0f, 0.75f, rsize.x() + 10.0f, NULL);

		m_widget_multi_list->clear();

		return;

	}
	
	if (widget == m_widget_filter){
		EResourcesInfoCard::Instance()->HideInfoCard();

		if(m_status == WIDGET_HALF_LEFT){
			//highlightButton(BUTTON_FILTER);
			m_action = WIDGET_HALF2FULL;
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_canvas, osgNewWidget::ANIM_PROPERTY_POS_Y,
				0.0f, 0.35f, 0.0f, this);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_left, osgNewWidget::ANIM_PROPERTY_POS_Y,
				0.0f, 0.35f, m_widget_canvas->getSize().y() - 20, NULL);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_right, osgNewWidget::ANIM_PROPERTY_POS_Y,
					0.0f, 0.35f, m_widget_canvas->getSize().y() - 20, NULL);

			g_vsimapp->getBrandingManager()->LowerMoveUp(m_widget_canvas->getSize().y() - g_settings.m_bar_size_2);
		}
		else if(m_status == WIDGET_FULL_LEFT){
			m_action = WIDGET_FULL2HALF;
			//unhighlightButton(BUTTON_FILTER);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_canvas, osgNewWidget::ANIM_PROPERTY_POS_Y,
				0.0f, 0.35f, -m_widget_canvas->getSize().y() + g_settings.m_bar_size_2, this);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_left, osgNewWidget::ANIM_PROPERTY_POS_Y,
				0.0f, 0.35f, g_settings.m_bar_size_2-20.0f, NULL);
			osgNewWidget::WidgetAnimator::instance()->add(m_widget_hide_right, osgNewWidget::ANIM_PROPERTY_POS_Y,
					0.0f, 0.35f, g_settings.m_bar_size_2-20.0f, NULL);

			g_vsimapp->getBrandingManager()->LowerMoveDown(m_widget_canvas->getSize().y() - g_settings.m_bar_size_2);
		}

		return;
	}
	if(widget == m_widget_filter_cancel){
		EResourcesInfoCard::Instance()->HideInfoCard();
		setWidgets();
		updateFilteredLists();

	}
	if (widget == m_widget_search_label ){
		EResourcesInfoCard::Instance()->HideInfoCard();
		m_category_list.m_expand = false;
		m_title_list.m_expand = false;
		m_type_list.m_expand = false;
		m_file_list_selection = LIST_SEARCH;
		highlightButton(BUTTON_SEARCH_LABEL);
		drawMultiRight();
		return;
	}
	if (widget == m_widget_search_input ){
		EResourcesInfoCard::Instance()->HideInfoCard();
		highlightButton(BUTTON_SEARCH_INPUT);
		searchInputButton();
		m_category_list.m_expand = false;
		m_title_list.m_expand = false;
		m_type_list.m_expand = false;
		m_file_list_selection = LIST_SEARCH;
		highlightButton(BUTTON_SEARCH_LABEL);
		unhighlightButton(BUTTON_SEARCH_INPUT);
		drawMultiRight();
		return;
	}
	if (widget == m_widget_search_cancel){
		EResourcesInfoCard::Instance()->HideInfoCard();
		highlightButton(BUTTON_SEARCH_CANCEL);
		m_widget_search_input->setLabel("Click to Input for Search");
		UpdateSearch();
		unhighlightButton(BUTTON_SEARCH_CANCEL);
		unhighlightButton(BUTTON_SEARCH_LABEL);

		return;
	}
	if(widget == m_widget_bar_ratio){
		ajustBarRatioButton();

		return;
	}
	
	if (widget == m_widget_add_left){
		EResourcesInfoCard::Instance()->HideInfoCard();
		highlightButton(BUTTON_ADD);
		addButtonLeft();//add one node into the list
		unhighlightButton(BUTTON_ADD);
		return;
	}
	
	if (widget == m_widget_remove_left)
    {
		EResourcesInfoCard::Instance()->HideInfoCard();
		highlightButton(BUTTON_DELETE);
        removeButton();
		unhighlightButton(BUTTON_DELETE);
		return;

    }
	
	if (widget == m_widget_edit_left){
		EResourcesInfoCard::Instance()->HideInfoCard();
		highlightButton(BUTTON_EDIT);
		editButton();
		unhighlightButton(BUTTON_EDIT);
		return;
	}

	if (widget == m_widget_add_right){
		EResourcesInfoCard::Instance()->HideInfoCard();
		highlightButton(BUTTON_ADD);
		addButtonRight();
		unhighlightButton(BUTTON_ADD);
		return;
	}
	
	if (widget == m_widget_remove_right)
    {
		EResourcesInfoCard::Instance()->HideInfoCard();
		highlightButton(BUTTON_DELETE);
        removeButton();
		unhighlightButton(BUTTON_DELETE);
		return;
		
    }
	
	if (widget == m_widget_edit_right){
		EResourcesInfoCard::Instance()->HideInfoCard();
		highlightButton(BUTTON_EDIT);
		editButton();
		unhighlightButton(BUTTON_EDIT);
		return;
	}
	
	if (widget == m_widget_global){
		EResourcesInfoCard::Instance()->HideInfoCard();
		m_category_list.m_expand = false;
		m_title_list.m_expand = false;
		m_type_list.m_expand = false;
		m_file_list_selection = LIST_GLOBAL_RIGHT;
		m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_global_offset);
		highlightButton(BUTTON_GLOBAL);
		drawMultiRight();
		return;
	}
	
	if (widget == m_widget_local){
		EResourcesInfoCard::Instance()->HideInfoCard();
		m_category_list.m_expand = false;
		m_title_list.m_expand = false;
		m_type_list.m_expand = false;
		m_file_list_selection = LIST_LOCAL_RIGHT;
		m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_local_offset);
		highlightButton(BUTTON_LOCAL);
		drawMultiRight();
		return;
	}
	
	if (widget == m_widget_title){
		EResourcesInfoCard::Instance()->HideInfoCard();
		m_category_list.m_expand = false;
		m_title_list.m_expand = false;
		m_type_list.m_expand = false;
		m_file_list_selection = LIST_TITLE;
		m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_title_offset);
		highlightButton(BUTTON_TITLE);
		drawMultiRight();
		return;
	}
	
	if (widget == m_widget_category){
		EResourcesInfoCard::Instance()->HideInfoCard();
		m_category_list.m_expand = false;
		m_title_list.m_expand = false;
		m_type_list.m_expand = false;
		m_file_list_selection = LIST_CATEGORY;
		m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_category_offset);
		highlightButton(BUTTON_CATEGORY);
		drawMultiRight();
		return;
	}
	
	if (widget == m_widget_filetype){
		EResourcesInfoCard::Instance()->HideInfoCard();
		m_category_list.m_expand = false;
		m_title_list.m_expand = false;
		m_type_list.m_expand = false;
		m_file_list_selection = LIST_TYPE;
		m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_type_offset);
		highlightButton(BUTTON_TYPE);
		drawMultiRight();
		return;
	}

}



void EResourcesListEditor::handleMouseDoubleClick(osgNewWidget::Widget* widget){

	if(m_list_over == LIST_CATEGORY){
		if(m_category_list.m_expand == false){
			m_category_list.m_expand = true;
			m_widget_multi_list->m_subcanvas->setContentOffset(osg::Vec2f(0,0));
			drawMultiRight();
		}
		else{
			showEResources(true);
		}
	}

	else if(m_list_over == LIST_TITLE){
		if(m_title_list.m_expand == false){
			m_title_list.m_expand = true;
			m_widget_multi_list->m_subcanvas->setContentOffset(osg::Vec2f(0,0));
			drawMultiRight();
		}
		else{
			showEResources(true);
		}
	}

	else if(m_list_over == LIST_TYPE){
		if(m_type_list.m_expand == false){
			m_type_list.m_expand = true;
			m_widget_multi_list->m_subcanvas->setContentOffset(osg::Vec2f(0,0));
			drawMultiRight();
		}
		else{
			showEResources(true);
		}
	}

	else if(m_list_over != LIST_NOSELECTION){
			showEResources(true);
	}

}

void EResourcesListEditor::UnselectAll(){

	m_eresources_local_list_left->SetSelect(-1);
	m_eresources_global_list_left->SetSelect(-1);
	m_eresources_local_list_right->SetSelect(-1);
	m_eresources_global_list_right->SetSelect(-1);
	
	m_category_list.SetSelect(-1);
	m_title_list.SetSelect(-1);
	m_type_list.SetSelect(-1);

	size_t list_size = m_category_list.m_eresources_list_vector.size();
	for (size_t i = 0; i < list_size; i++)
		m_category_list.m_eresources_list_vector[i]->SetSelect(-1); 

	list_size = m_title_list.m_eresources_list_vector.size();
	for (size_t i = 0; i < list_size; i++)
		m_title_list.m_eresources_list_vector[i]->SetSelect(-1); 

	list_size = m_type_list.m_eresources_list_vector.size();
	for (size_t i = 0; i < list_size; i++)
		m_type_list.m_eresources_list_vector[i]->SetSelect(-1); 

	//These are not included in UnselectAll();

	//m_category_list.SetSelect(-1);
	//m_title_list.SetSelect(-1);
	//m_type_list.SetSelect(-1);

	drawMultiLeft();
	drawMultiRight();
	EResourcesInfoCard::Instance()->HideInfoCard();
	
}

bool EResourcesListEditor::SelectResource(osgNewWidget::Widget* widget){
	//iterate throught the current list and check if any widget is selected, if selected mark it.

	if(m_status == WIDGET_HALF_LEFT || m_status == WIDGET_FULL_LEFT){

		for (size_t i = 0; i < m_eresources_local_list_left->m_widgets.size(); i++)
		{
				if (m_eresources_local_list_left->m_widgets[i].get() == widget)
				{
					 m_list_select = LIST_LOCAL_LEFT;
					 UnselectAll();
					 m_eresources_local_list_left->SetSelect(i);
					 drawMultiLeft();//make the display of current widget as selected;
					 EResourcesInfoCard::Instance()->ShowInfoCard(m_eresources_local_list_left->m_list[i]);
					 return true;
				}
		}

		for (size_t i = 0; i < m_eresources_global_list_left->m_widgets.size(); i++)
		{
				if (m_eresources_global_list_left->m_widgets[i].get() == widget)
				{
					m_list_select = LIST_GLOBAL_LEFT;
					UnselectAll();
					m_eresources_global_list_left->SetSelect(i);
					drawMultiLeft();
					EResourcesInfoCard::Instance()->ShowInfoCard(m_eresources_global_list_left->m_list[i]);

					return true;
				}
		}
	}

	if(m_status == WIDGET_HALF_RIGHT)
	{
		switch(m_file_list_selection){
		case LIST_GLOBAL_RIGHT:

		for (size_t i = 0; i < m_eresources_global_list_right->m_widgets.size(); i++)
		{
				if (m_eresources_global_list_right->m_widgets[i].get() == widget)
				{
					 m_list_select = LIST_GLOBAL_RIGHT;
					 UnselectAll();
					 m_eresources_global_list_right->SetSelect(i);
					 drawMultiRight();
					 EResourcesInfoCard::Instance()->ShowInfoCard(m_eresources_global_list_right->m_list[i]);
					 return true;
				}
		}

		break;

		case LIST_LOCAL_RIGHT:
		
		for (size_t i = 0; i < m_eresources_local_list_right->m_widgets.size(); i++)
		{
				if (m_eresources_local_list_right->m_widgets[i].get() == widget)
				{
					 m_list_select = LIST_LOCAL_RIGHT;
					 UnselectAll();
					 m_eresources_local_list_right->SetSelect(i);
					 drawMultiRight();
					 EResourcesInfoCard::Instance()->ShowInfoCard(m_eresources_local_list_right->m_list[i]);
					 return true;
				}
		}

		break;

		case LIST_SEARCH:
		for (size_t i = 0; i < m_eresources_searched_list->m_widgets.size(); i++)
		{
				if (m_eresources_searched_list->m_widgets[i].get() == widget)
				{
					 m_list_select = LIST_SEARCH;
					 UnselectAll();
					 m_eresources_searched_list->SetSelect(i);
					 drawMultiRight();
					 EResourcesInfoCard::Instance()->ShowInfoCard(m_eresources_searched_list->m_list[i]);
					 return true;
				}
		}

		break;

		case LIST_CATEGORY:

		if(m_category_list.m_expand == false){

			for (size_t i = 0; i < m_category_list.m_category_vector.size(); i++)
			{
				if (m_category_list.m_widgets[i].get() == widget)
				{
					m_list_select = LIST_CATEGORY;
					UnselectAll();
					m_category_list.SetSelect(i);
					drawMultiRight();
					return true;
				}
			}
		}
		else{
			size_t list_size = m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_list.size();
			for (size_t i = 0; i < list_size; i++){
				if(m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_widgets[i].get() == widget){
					m_list_select = LIST_CATEGORY;
					UnselectAll();
					m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->SetSelect(i); 
					EResourcesInfoCard::Instance()->ShowInfoCard(m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_list[i]);
					drawMultiRight();

					return true;
				}
			}
			
		}

		break;

		case LIST_TITLE:
		if(m_title_list.m_expand == false){

			for (size_t i = 0; i < m_title_list.m_title_vector.size(); i++)
			{
				if (m_title_list.m_widgets[i].get() == widget)
				{
					m_list_select = LIST_TITLE;
					UnselectAll();
					m_title_list.SetSelect(i);
					drawMultiRight();

					return true;
				}
			}
		}
		else{
			size_t list_size = m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_list.size();
			for (size_t i = 0; i < list_size; i++){
				if(m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_widgets[i].get() == widget){
					m_list_select = LIST_TITLE;
					UnselectAll();
					m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->SetSelect(i); 
					drawMultiRight();
					EResourcesInfoCard::Instance()->ShowInfoCard(m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_list[i]);
					return true;
				}
			}
			
		}

		break;

		case LIST_TYPE:
		if(m_type_list.m_expand == false){

			for (size_t i = 0; i < m_type_list.m_type_vector.size(); i++)
			{
				if (m_type_list.m_widgets[i].get() == widget)
				{
					m_list_select = LIST_TYPE;
					UnselectAll();
					m_type_list.SetSelect(i);
					drawMultiRight();

					return true;
				}
			}
		}
		else{
			size_t list_size = m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_list.size();
			for (size_t i = 0; i < list_size; i++){
				if(m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_widgets[i].get() == widget){
					m_list_select = LIST_TYPE;
					UnselectAll();
					m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->SetSelect(i); 
					drawMultiRight();
					EResourcesInfoCard::Instance()->ShowInfoCard(m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_list[i]);
					return true;
				}
			}
			
		}
		break;
	}//endswitch
	}//endif
	
	return false;
}

void EResourcesListEditor::removeButton()
{
	
	switch(m_list_select){
		case LIST_GLOBAL_LEFT:
			if(m_eresources_global_list_left->GetSelect() >= 0)
			{
				MessageDialog md;
				bool result = md.show("Delete the Resource?", MESSAGE_DIALOG_YES_NO);
				if (!result)
					return;

				//remove the resource from the centrallist first
				for(unsigned int i = 0; i < m_eresources_central_list->m_list.size(); i++)
				{
					if(m_eresources_central_list->m_list[i] == m_eresources_global_list_left->m_list[m_eresources_global_list_left->GetSelect()])
					{
						EResourcesNodeVector::iterator er_to_delete;
						WidgetVector::iterator wd_to_delete;
						er_to_delete = m_eresources_central_list->m_list.begin() + i;
						wd_to_delete = m_eresources_central_list->m_widgets.begin() + i;
						m_eresources_central_list->m_list.erase(er_to_delete);
						m_eresources_central_list->m_widgets.erase(wd_to_delete);
						updateEndLists();
						m_filter.setWidgets();//need to rebuild the filter tags after adding a new resource
						return;
					}
				}

			}
			
			break;
		case LIST_LOCAL_LEFT:
			if(m_eresources_local_list_left->GetSelect() >= 0)
			{
				MessageDialog md;
				bool result = md.show("Delete the Resource?", MESSAGE_DIALOG_YES_NO);
				if (!result)
					return;

				//remove the resource from the centrallist first
				for(unsigned int i = 0; i < m_eresources_central_list->m_list.size(); i++)
				{
					if(m_eresources_central_list->m_list[i] == m_eresources_local_list_left->m_list[m_eresources_local_list_left->GetSelect()])
					{
						EResourcesNodeVector::iterator er_to_delete;
						WidgetVector::iterator wd_to_delete;
						er_to_delete = m_eresources_central_list->m_list.begin() + i;
						wd_to_delete = m_eresources_central_list->m_widgets.begin() + i;
						m_eresources_central_list->m_list.erase(er_to_delete);
						m_eresources_central_list->m_widgets.erase(wd_to_delete);
						updateEndLists();
						m_filter.setWidgets();//need to rebuild the filter tags after adding a new resource
						return;
					}
				}
			}
			
			break;
		case LIST_GLOBAL_RIGHT:
			if(m_eresources_global_list_right->GetSelect() >= 0)
			{
				MessageDialog md;
				bool result = md.show("Delete the Resource?", MESSAGE_DIALOG_YES_NO);
				if (!result)
					return;

				//remove the resource from the centrallist first
				for(unsigned int i = 0; i < m_eresources_central_list->m_list.size(); i++)
				{
					if(m_eresources_central_list->m_list[i] == m_eresources_global_list_right->m_list[m_eresources_global_list_right->GetSelect()])
					{
						EResourcesNodeVector::iterator er_to_delete;
						WidgetVector::iterator wd_to_delete;
						er_to_delete = m_eresources_central_list->m_list.begin() + i;
						wd_to_delete = m_eresources_central_list->m_widgets.begin() + i;
						m_eresources_central_list->m_list.erase(er_to_delete);
						m_eresources_central_list->m_widgets.erase(wd_to_delete);
						updateEndLists();
						m_filter.setWidgets();//need to rebuild the filter tags after adding a new resource
						return;
					}
				}
			}
			
			break;
		case LIST_LOCAL_RIGHT:
			if(m_eresources_local_list_right->GetSelect() >= 0)
			{
				MessageDialog md;
				bool result = md.show("Delete the Resource?", MESSAGE_DIALOG_YES_NO);
				if (!result)
					return;

				
				//remove the resource from the centrallist first
				for(unsigned int i = 0; i < m_eresources_central_list->m_list.size(); i++)
				{
					if(m_eresources_central_list->m_list[i] == m_eresources_local_list_right->m_list[m_eresources_local_list_right->GetSelect()])
					{
						EResourcesNodeVector::iterator er_to_delete;
						WidgetVector::iterator wd_to_delete;
						er_to_delete = m_eresources_central_list->m_list.begin() + i;
						wd_to_delete = m_eresources_central_list->m_widgets.begin() + i;
						m_eresources_central_list->m_list.erase(er_to_delete);
						m_eresources_central_list->m_widgets.erase(wd_to_delete);
						updateEndLists();
						m_filter.setWidgets();//need to rebuild the filter tags after adding a new resource
						return;
					}
				}

			}
			break;
		case LIST_SEARCH:
			if(m_eresources_searched_list->GetSelect() >= 0)
			{
				MessageDialog md;
				bool result = md.show("Delete the Resource?", MESSAGE_DIALOG_YES_NO);
				if (!result)
					return;

				//remove the resource from the centrallist first
				for(unsigned int i = 0; i < m_eresources_central_list->m_list.size(); i++)
				{
					if(m_eresources_central_list->m_list[i] == m_eresources_searched_list->m_list[m_eresources_searched_list->GetSelect()])
					{
						EResourcesNodeVector::iterator er_to_delete;
						WidgetVector::iterator wd_to_delete;
						er_to_delete = m_eresources_central_list->m_list.begin() + i;
						wd_to_delete = m_eresources_central_list->m_widgets.begin() + i;
						m_eresources_central_list->m_list.erase(er_to_delete);
						m_eresources_central_list->m_widgets.erase(wd_to_delete);
						updateEndLists();
						m_filter.setWidgets();//need to rebuild the filter tags after adding a new resource
						return;
					}
				}
			}
			
			break;
		case LIST_CATEGORY:
			if(m_category_list.m_expand == true){//delete the selected node
				if(m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->GetSelect() >= 0)
				{
					MessageDialog md;
					bool result = md.show("Delete the Resource?", MESSAGE_DIALOG_YES_NO);
					if (!result)
						return;
					//remove the resource from the centrallist first
					for(unsigned int i = 0; i < m_eresources_central_list->m_list.size(); i++)
					{
						if( *(m_eresources_central_list->m_list[i]) == 
							*(m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->GetSelectedNode()) )
						{
							EResourcesNodeVector::iterator er_to_delete;
							WidgetVector::iterator wd_to_delete;
							er_to_delete = m_eresources_central_list->m_list.begin() + i;
							wd_to_delete = m_eresources_central_list->m_widgets.begin() + i;
							m_eresources_central_list->m_list.erase(er_to_delete);
							m_eresources_central_list->m_widgets.erase(wd_to_delete);
							updateEndLists();
							m_filter.setWidgets();//need to rebuild the filter tags after adding a new resource
							return;
						}
					}
				}
				
			}
			else{
				if(m_category_list.GetSelect() >= 0){
					
					MessageDialog md;
					bool result = md.show("Delete the Whole Category?", MESSAGE_DIALOG_YES_NO);
					if (!result)
						return;
					//remove all the cateogry nodes in central list
					erCategory& toDelete = m_category_list.GetSelectedCategory();
					//appy marker
					for(unsigned int i = 0; i < m_eresources_central_list->m_list.size(); i++)
						if(m_eresources_central_list->m_list[i]->m_category == toDelete){
							std::string marker = "*******";
							m_eresources_central_list->m_list[i]->m_category.Name = marker;
							m_eresources_central_list->m_widgets[i]->setLabel(marker);
						}
					//apply remove_if using predicate cmpCategory, the predicate use the marker "*******"
					m_eresources_central_list->m_list.erase(std::remove_if(m_eresources_central_list->m_list.begin(),
						m_eresources_central_list->m_list.end(), cmpCategory),
						m_eresources_central_list->m_list.end());
					m_eresources_central_list->m_widgets.erase(std::remove_if(m_eresources_central_list->m_widgets.begin(),
						m_eresources_central_list->m_widgets.end(), cmpWidget),
						m_eresources_central_list->m_widgets.end());
					updateEndLists();
					m_filter.setWidgets();//need to rebuild the filter tags after adding a new resource
					
					return;
				}
			}
			break;
		case LIST_TITLE:
			if(m_title_list.m_expand == true){//delete the selected node
				if(m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->GetSelect() >= 0)
				{
					MessageDialog md;
					bool result = md.show("Delete the Resource?", MESSAGE_DIALOG_YES_NO);
					if (!result)
						return;

					//remove the resource from the centrallist first
					for(unsigned int i = 0; i < m_eresources_central_list->m_list.size(); i++)
					{
						if( *(m_eresources_central_list->m_list[i]) == 
							*(m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->GetSelectedNode()) )
						{
							EResourcesNodeVector::iterator er_to_delete;
							WidgetVector::iterator wd_to_delete;
							er_to_delete = m_eresources_central_list->m_list.begin() + i;
							wd_to_delete = m_eresources_central_list->m_widgets.begin() + i;
							m_eresources_central_list->m_list.erase(er_to_delete);
							m_eresources_central_list->m_widgets.erase(wd_to_delete);
							updateEndLists();
							m_filter.setWidgets();//need to rebuild the filter tags after adding a new resource
	
							return;
						}
					}
				}
				
			}
			break;
		case LIST_TYPE:
			if(m_type_list.m_expand == true){//delete the selected node
				if(m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->GetSelect() >= 0)
				{
					MessageDialog md;
					bool result = md.show("Delete the Resource?", MESSAGE_DIALOG_YES_NO);
					if (!result)
						return;

					for(unsigned int i = 0; i < m_eresources_central_list->m_list.size(); i++)
					{
						if( *(m_eresources_central_list->m_list[i]) == 
							*(m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->GetSelectedNode()) )
						{
							EResourcesNodeVector::iterator er_to_delete;
							WidgetVector::iterator wd_to_delete;
							er_to_delete = m_eresources_central_list->m_list.begin() + i;
							wd_to_delete = m_eresources_central_list->m_widgets.begin() + i;
							m_eresources_central_list->m_list.erase(er_to_delete);
							m_eresources_central_list->m_widgets.erase(wd_to_delete);
							updateEndLists();
							m_filter.setWidgets();//need to rebuild the filter tags after adding a new resource
							
							return;
						}
					}
				}
				
			}
			break;
	}
}

void EResourcesListEditor::addButtonLeft(){

	NewEResourceDialog dlg("Add a New Resource", m_category_list.GetCategoryVector());

	int result = dlg.ShowModal();
    if (result != wxID_OK)
        return;
	
    std::string l_title = dlg.getName();
    std::string l_filepath = dlg.getFilePath();
	int l_global = dlg.getGlobal();
	int l_minyear = dlg.getMinYear();
	int l_maxyear = dlg.getMaxYear();
	int l_reposition = dlg.getReposition();
	int l_launch = dlg.getLaunch();
	float l_local_range = dlg.getLocalRange();
	int l_ertype = dlg.getResourceType();
	erCategory l_category = dlg.m_category;
	std::string l_filetype;
	if(l_ertype == 0)
		l_filetype = Util::getExtension(Util::addExtensionIfNotExist(l_filepath,"unknown"));
	else if(l_ertype == 1)
		l_filetype = "URL";
	else if(l_ertype == 2)
		l_filetype = "NOTE";

	std::string l_discription = dlg.getDiscription();
	std::string l_authors = dlg.getAuthor();

	int l_copyright_choice = dlg.getCopyRight();


	osg::ref_ptr<EResourcesNode> newnode = 
		m_eresources_central_list->addNode(l_title, l_filetype, l_filepath, l_discription, l_authors,
		l_copyright_choice,l_category, l_global, l_minyear, l_maxyear, l_local_range, l_reposition, l_launch, l_ertype);
	
	EResourcesInfoCard::Instance()->ShowInfoCard(newnode);

	updateEndLists();

	m_filter.setWidgets();//need to rebuild the filter tags after adding a new resource

}

void EResourcesListEditor::addButtonRight(){
	switch(m_file_list_selection){
		case LIST_CATEGORY:
			/*if(m_category_list.m_expand == false){
				m_category_list.NewCategory();
				return;
			}
			else{
			*/
				addButtonLeft();
			//}
			break;
		default:
			addButtonLeft();
			break;
	}
}
void EResourcesListEditor::editButton(){
	
	int index;
	EResourcesNode *tempnode = 0;
	
	switch(m_list_select){
		case LIST_GLOBAL_LEFT:
			index = m_eresources_global_list_left->GetSelect();
			if(index < 0)//nothing selected
				return;
			tempnode = m_eresources_global_list_left->m_list[index];
			break;
		case LIST_LOCAL_LEFT:
			index = m_eresources_local_list_left->GetSelect();
			if(index < 0)//nothing selected
				return;
			tempnode = m_eresources_local_list_left->m_list[index];
			break;
		case LIST_GLOBAL_RIGHT:
			index = m_eresources_global_list_right->GetSelect();
			if(index < 0)//nothing selected
				return;
			tempnode = m_eresources_global_list_right->m_list[index];
			break;
		case LIST_LOCAL_RIGHT:
			index = m_eresources_local_list_right->GetSelect();
			if(index < 0)//nothing selected
				return;
			tempnode = m_eresources_local_list_right->m_list[index];
			break;
		case LIST_SEARCH:
			index = m_eresources_searched_list->GetSelect();
			if(index < 0)//nothing selected
				return;
			tempnode = m_eresources_searched_list->m_list[index];
			break;
		case LIST_CATEGORY:
			if(m_category_list.m_expand == false){
				m_category_list.EditCategory();
				return;
			}
			else{
				index = m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->GetSelect();
				if(index < 0)//nothing selected
					return;
				tempnode = m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_list[index];
			}
			
			break;
		case LIST_TITLE:
			if(m_title_list.m_expand == true){
				index = m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->GetSelect();
				if(index < 0)//nothing selected
					return;
				tempnode = m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_list[index];
			}
			else
				return;
			break;
		case LIST_TYPE:
			if(m_type_list.m_expand == true){
				index = m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->GetSelect();
				if(index < 0)//nothing selected
					return;
				tempnode = m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_list[index];
			}
			else
				return;
			break;
		default:
			return;
	}

	NewEResourceDialog dlg("Edit the Resource",m_category_list.GetCategoryVector());

	dlg.setName(tempnode->m_name);
	dlg.setFilePath(tempnode->m_filepath);
	dlg.setDiscription(tempnode->m_description);
	dlg.setAuthor(tempnode->m_authors);
	dlg.setCopyRight(tempnode->m_copyright);
	dlg.setGlobal(tempnode->m_global);
	dlg.setMinYear(tempnode->m_min_year);
	dlg.setMaxYear(tempnode->m_max_year);
	dlg.setReposition(tempnode->m_reposition);
	dlg.setLaunch(tempnode->m_launch);
	dlg.setLocalRange(tempnode->m_local_range);
	dlg.setResourceType(tempnode->m_ertype);
	dlg.setCategory(tempnode->m_category);

	int result = dlg.ShowModal();
    if (result != wxID_OK)
        return;
	
	std::string l_title = dlg.getName();
    std::string l_filepath = dlg.getFilePath();
	std::string l_author = dlg.getAuthor();
	int l_ertype = dlg.getResourceType();
	int l_copyright = dlg.getCopyRight();
	std::string l_filetype;
	switch(l_ertype){
		case 0:
			l_filetype = Util::getExtension(Util::addExtensionIfNotExist(l_filepath,"unknown"));
			break;
		case 1:
			l_filetype = "URL";
			break;
		case 2:
			l_filetype = "NOTE";
	}
	std::string l_discription =dlg.getDiscription();
	int l_global = dlg.getGlobal();
	int l_minyear = dlg.getMinYear();
	int l_maxyear = dlg.getMaxYear();
	int l_reposition = dlg.getReposition();
	int l_launch = dlg.getLaunch();
	float l_local_range = dlg.getLocalRange();
	erCategory l_category = dlg.m_category;

	//locate the tempnode in the central node, set m_selection
	for(unsigned int i = 0; i < m_eresources_central_list->m_list.size(); i++)
		if(*(m_eresources_central_list->m_list[i]) == *tempnode){
			m_eresources_central_list->SetSelect(i);
			break;
		}
	//then change the node

	osg::ref_ptr<EResourcesNode> newnode = 
		m_eresources_central_list->updateNode(l_title, l_filetype, l_filepath, l_discription, l_author, l_copyright,
		l_category, l_global, l_minyear, l_maxyear, l_local_range, l_reposition, l_launch,l_ertype);

	EResourcesInfoCard::Instance()->ShowInfoCard(newnode);
	updateEndLists();
	m_filter.setWidgets();
}

void EResourcesListEditor::searchInputButton(){
	SearchDialog dlg;
	int result = dlg.ShowModal();
    if (result != wxID_OK)
        return;
	std::string l_search_string = dlg.getSearchString();
	m_widget_search_input->setLabel(l_search_string);
	highlightButton(BUTTON_SEARCH_LABEL);
	UpdateSearch();
}

void EResourcesListEditor::UpdateSearch(){
	m_eresources_searched_list->Clear();
	std::string l_search_string  = m_widget_search_input->getLabel();
	if(l_search_string == ""||l_search_string == "Click to Input for Search")
		return;
	//first extract words
	std::vector<std::string> SearchWords;
	Util::extraceWords(l_search_string, SearchWords);
	std::vector<int> Score(m_eresources_central_list->m_list.size());
	if(Score.size() == 0)
		return;
	for(unsigned int i = 0; i < Score.size(); i++)
		Score[i] = 0;
	//then look for appearance of words in title, description and author, for links also url
	for(unsigned int i = 0; i < Score.size(); i++){
		std::vector<std::string> TitleWords, DescriptionWords, AuthorWords, SourceWords;
		Util::extraceWords(m_eresources_central_list->m_list[i]->m_name, TitleWords);
		Util::extraceWords(m_eresources_central_list->m_list[i]->m_description, DescriptionWords);
		Util::extraceWords(m_eresources_central_list->m_list[i]->m_authors, AuthorWords);
		if(m_eresources_central_list->m_list[i]->m_ertype!=2)
			Util::extraceWords(m_eresources_central_list->m_list[i]->m_filepath, SourceWords);
		//update score by searching for containment of word
		Score[i] += Util::numContain(TitleWords, SearchWords);
		Score[i] += Util::numContain(DescriptionWords, SearchWords);
		Score[i] += Util::numContain(AuthorWords, SearchWords);
		if(m_eresources_central_list->m_list[i]->m_ertype!=2)
			Score[i] += Util::numContain(SourceWords, SearchWords);
	}
	//rank all that have a 0+ score, put into m_eresources_searched_list
	int biggest = 1, biggestID = 0;
	while(biggest){
		biggest = Score[0];
		biggestID = 0;
		for(unsigned int i = 0; i < Score.size(); i++)
		{//get the biggest score and id of the item in the list
			if(biggest < Score[i])
			{
				biggest = Score[i];
				biggestID = i;
			}
		}

		if(biggest){//if the biggest score is bigger than 0
			m_eresources_searched_list->m_list.push_back(m_eresources_central_list->m_list[biggestID]);//shallow
			m_eresources_searched_list->m_widgets.push_back(new VSLabel(m_eresources_central_list->m_widgets[biggestID]->getLabel()));//deep
			Score[biggestID] = 0;//set the score of this one as 0 so will not be counted in looking for the next one
		}
		else//if the biggest score is 0, done
			break;
	}
}

void EResourcesListEditor::animationEnded(osgNewWidget::Widget* widget)
{
    if (widget == m_widget_canvas)
    {
        if (m_action == WIDGET_HALF2CLOSED)
        {
            m_widget_hide_left->setLabel("^");
			m_widget_hide_right->setLabel("^");
			m_status = (m_status == WIDGET_HALF_LEFT) ? WIDGET_CLOSED_LEFT: WIDGET_CLOSED_RIGHT;
        }
        else if(m_action == WIDGET_CLOSED2HALF)
        {
            m_widget_hide_left->setLabel("v");
			m_widget_hide_right->setLabel("v");
            m_status = (m_status == WIDGET_CLOSED_LEFT) ? WIDGET_HALF_LEFT: WIDGET_HALF_RIGHT;
            resetAnchors();
        }
		else if(m_action == WIDGET_LEFT2RIGHT)
		{
			m_status = WIDGET_HALF_RIGHT;
		}
		else if(m_action == WIDGET_RIGHT2LEFT)
		{
			m_status = WIDGET_HALF_LEFT;
		}
		else if(m_action == WIDGET_HALF2FULL){
			m_status = WIDGET_FULL_LEFT; //the right does not have full mode for now
		}
		else if(m_action == WIDGET_FULL2HALF){
			m_status = (m_status == WIDGET_FULL_LEFT) ? WIDGET_HALF_LEFT: WIDGET_HALF_RIGHT;
		}
		
    }
}

void EResourcesListEditor::resetAnchors()
{
	
 //   osgNewWidget::Canvas* rootCanvas = osgNewWidget::WidgetManager::instance()->getRoot();
 //   rootCanvas->setHAnchor(m_widget_canvas.get(), osgNewWidget::ANCHOR_L);
 //   rootCanvas->setVAnchor(m_widget_canvas.get(), osgNewWidget::ANCHOR_B);
 //   rootCanvas->setVAnchor(m_widget_hide_left.get(), osgNewWidget::ANCHOR_B);
	//rootCanvas->setVAnchor(m_widget_hide_right.get(), osgNewWidget::ANCHOR_B);
	//
}


osg::Vec2f EResourcesListEditor::getPosition()
{
    return m_widget_canvas->getPosition();
}


void EResourcesListEditor::drawMultiLeft(){

	//change filter tab color based on the selection of tags
	if(m_filter.IsFilterOn())//filter on
		m_widget_filter->setColor(osg::Vec4f(0,0.5,0, 0.5));
	else
		m_widget_filter->setColor(osg::Vec4f(0.7, 0.2, 0.2, 0.5));

	//get current camera position
	osg::Matrixd current_view_matrix;
	osg::Vec3f camera_position;
	current_view_matrix = g_viewer->getCameraManipulator()->getMatrix();
	camera_position = current_view_matrix.getTrans(); 
	
	//handle the local multibar
	m_leftbar_local_offset = m_widget_multi_local->m_subcanvas->getContentOffset();
	m_widget_multi_local->clear();

	int list_size = m_eresources_local_list_left->m_widgets.size();

	osg::ref_ptr<VSLabel> current_label;
	osg::Vec3f current_position;
	erCategory current_category;
	float Red, Green, Blue;
	float current_distance;

	for (int i = 0; i != list_size; i++){
		current_position = m_eresources_local_list_left->m_list[i]->m_camera_position;
		current_distance = (current_position - camera_position).length();
		if(current_distance >= m_eresources_local_list_left->m_list[i]->m_local_range){
			//out of view
			if(m_eresources_local_list_left->m_list[i]->m_inview == true){
				//previously in range, now out of range   
				m_eresources_local_list_left->m_list[i]->m_inview = false;
				//reset the offset
				m_leftbar_local_offset = osg::Vec2f(0.0f, 0.0f);
				//no more display of this infocard
				if(EResourcesInfoCard::Instance()->GetCurrentER() == m_eresources_local_list_left->m_list[i])
					EResourcesInfoCard::Instance()->HideInfoCard();
			}
		    continue;//do not display this one since the distance is larger than preset.
		}
		
		if(m_eresources_local_list_left->m_list[i]->m_inview == false)
		{
			//this means it used to be not in the view, not it enters the view
			if(m_eresources_local_list_left->m_list[i]->getAutoLaunch() != 1)//auto launch on(0) or text(2)
			{	
				
					m_list_over = LIST_LOCAL_LEFT;
					m_list_select = LIST_LOCAL_LEFT;
					m_eresources_local_list_left->SetSelect(i);
					if(m_eresources_local_list_left->m_list[i]->getAutoLaunch() == 0)
						showEResources(true);//launch the file/url
					else
						showEResources(false);//only show expanded card without launch file

			}
			//reset the offset
			m_leftbar_local_offset = osg::Vec2f(0.0f, 0.0f);
		}
		m_eresources_local_list_left->m_list[i]->m_inview = true;

		current_label = m_eresources_local_list_left->m_widgets[i];
		current_category = m_eresources_local_list_left->m_list[i]->m_category;
		current_label->setHandler(this);
		std::string label; int num_line; bool full;
		Util::MultilineGen(m_eresources_local_list_left->m_list[i]->m_name,label,num_line, 12, 3, full, 1);
		current_label->setLabel(label);
		//if mouse over, enlarge
		if(m_eresources_local_list_left->GetOver() == i){			
			current_label->setSize(osg::Vec2f(95.0f, 95.0f));
			current_label->setFontSize(14);
			current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
		}
		else{
			current_label->setSize(osg::Vec2f(80.0f, 80.0f));
			current_label->setFontSize(12);
			current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
		}

		//set color
		Red = current_category.Red/255.0;
		Green = current_category.Green/255.0;
		Blue = current_category.Blue/255.0;
		//change alpha, etc for selected items
		if(m_eresources_local_list_left->GetSelect() == i){	
			current_label->setColor(osg::Vec4f(Red,Green,Blue,1.0f));
			current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
			current_label->setBorderColor(osg::Vec4f(1,1,1,1));
		}
		else{
			current_label->setColor(osg::Vec4f(Red,Green,Blue,0.3f));
			current_label->setBorderColor(osg::Vec4f(0,0,0,0));
			current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
		}
		m_widget_multi_local->addWidget(current_label);
		//if the current resource is auto-reposition enabled, also display an icon on the top left corner
		if(m_eresources_local_list_left->m_list[i]->m_reposition == 0)//auto reposition on
		{
			osg::ref_ptr<VSLabel> reposition_marker;
			reposition_marker = new VSLabel("");
			reposition_marker->setFontSize(10);
			reposition_marker->setFontColor(osg::Vec4f(1,1,1,1));
			reposition_marker->setColor(osg::Vec4f(1,1,1,1));
			reposition_marker->setBorderColor(osg::Vec4f(0,0,0,0));
			if(m_eresources_local_list_left->GetOver() == i){
				reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
				reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 82.0f)));

			}
			else{
				reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
				reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 68.0f)));

			}
			m_widget_multi_local->addWidgetToPrev(reposition_marker);//must be add after the widget is added
		}
	}
	
	m_widget_multi_local->m_subcanvas->setContentOffset(m_leftbar_local_offset);

	//handel the global multi bar
	m_leftbar_global_offset = m_widget_multi_global->m_subcanvas->getContentOffset();
	m_widget_multi_global->clear();

	list_size = m_eresources_global_list_left->m_widgets.size();

	for (int i = 0; i != list_size; i++){
		current_label = m_eresources_global_list_left->m_widgets[i];
		current_category = m_eresources_global_list_left->m_list[i]->m_category;
		std::string label; int num_line; bool full;
		Util::MultilineGen(m_eresources_global_list_left->m_list[i]->m_name,label,num_line, 12, 3, full,1);
		current_label->setLabel(label);
		current_label->setHandler(this);
		
		if(m_eresources_global_list_left->GetOver() == i){
			current_label->setSize(osg::Vec2f(95.0f, 95.0f));
			current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
			current_label->setFontSize(14);
		}
		else{
			current_label->setSize(osg::Vec2f(80.0f, 80.0f));
			current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
			current_label->setFontSize(12);
		}
		
		//set color
		Red = current_category.Red/255.0;
		Green = current_category.Green/255.0;
		Blue = current_category.Blue/255.0;

		if(m_eresources_global_list_left->GetSelect() == i){	
			current_label->setColor(osg::Vec4f(Red,Green,Blue,1.0f));
			current_label->setBorderColor(osg::Vec4f(1,1,1,1));
			current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
		}
		else{
			current_label->setColor(osg::Vec4f(Red,Green,Blue,0.5f));
			current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
			current_label->setBorderColor(osg::Vec4f(0,0,0,0));
		}

		m_widget_multi_global->addWidget(current_label);
		//if the current resource is auto-reposition enabled, also display an icon on the top left corner
		if(m_eresources_global_list_left->m_list[i]->m_reposition == 0)//auto reposition on
		{
			osg::ref_ptr<VSLabel> reposition_marker;
			reposition_marker = new VSLabel("");
			reposition_marker->setFontSize(10);
			reposition_marker->setFontColor(osg::Vec4f(1,1,1,1));
			reposition_marker->setColor(osg::Vec4f(1,1,1,1));
			reposition_marker->setBorderColor(osg::Vec4f(0,0,0,0));
			if(m_eresources_global_list_left->GetOver() == i){
				reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
				reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 82.0f)));

			}
			else{
				reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
				reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 68.0f)));

			}
			m_widget_multi_global->addWidgetToPrev(reposition_marker);//must be add after the widget is added
		}
	}
	m_widget_multi_global->m_subcanvas->setContentOffset(m_leftbar_global_offset);
	
}

void EResourcesListEditor::drawMultiRight(){

	m_widget_multi_list->clear();
	
	int list_size, i;
	osg::ref_ptr<VSLabel> current_label;
	erCategory current_category;
	osg::Vec2f temp_offset;
	float Red, Green, Blue;

	switch(m_file_list_selection){
		case LIST_LOCAL_RIGHT:
			//update the m_eresources_local_list_right
			list_size = m_eresources_local_list_right->m_widgets.size();
			for (i = 0; i != list_size; i++){
				current_label = m_eresources_local_list_right->m_widgets[i];
				current_category = m_eresources_local_list_right->m_list[i]->m_category;
				std::string label; int num_line; bool full;
				Util::MultilineGen(m_eresources_local_list_right->m_list[i]->m_name,label,num_line, 12, 3, full,1);
				current_label->setLabel(label);
				current_label->setHandler(this);
				if(m_eresources_local_list_right->GetOver() == i){
  					current_label->setSize(osg::Vec2f(95.0f, 95.0f));
					current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
					current_label->setFontSize(14);
				}
				else{
					current_label->setSize(osg::Vec2f(80.0f, 80.0f));
					current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
					current_label->setFontSize(12);
					//set color
				}
				Red = current_category.Red/255.0;
				Green = current_category.Green/255.0;
				Blue = current_category.Blue/255.0;
				if(m_eresources_local_list_right->GetSelect() == i){	
					current_label->setColor(osg::Vec4f(Red,Green,Blue,1.0f));
					current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
					current_label->setBorderColor(osg::Vec4f(1,1,1,1));
				}
				else{
					current_label->setColor(osg::Vec4f(Red,Green,Blue,0.5f));
					current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
					current_label->setBorderColor(osg::Vec4f(0,0,0,0));
				}
				
				m_widget_multi_list->addWidget(current_label);
				//if the current resource is auto-reposition enabled, also display an icon on the top left corner
				if(m_eresources_local_list_right->m_list[i]->m_reposition == 0)//auto reposition on
				{
					osg::ref_ptr<VSLabel> reposition_marker;
					reposition_marker = new VSLabel("");
					reposition_marker->setFontSize(10);
					reposition_marker->setFontColor(osg::Vec4f(1,1,1,1));
					reposition_marker->setColor(osg::Vec4f(1,1,1,1));
					reposition_marker->setBorderColor(osg::Vec4f(0,0,0,0));
					if(m_eresources_local_list_right->GetOver() == i){
						reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
						reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 82.0f)));

					}
					else{
						reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
						reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 68.0f)));

					}
					m_widget_multi_list->addWidgetToPrev(reposition_marker);//must be add after the widget is added
				}

				m_rightbar_local_offset = m_widget_multi_list->m_subcanvas->getContentOffset();
				m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_local_offset);
			}//end for
			break;
		case LIST_GLOBAL_RIGHT:
			list_size = m_eresources_global_list_right->m_widgets.size();
			for (i = 0; i != list_size; i++){
				current_label = m_eresources_global_list_right->m_widgets[i];
				current_category = m_eresources_global_list_right->m_list[i]->m_category;
				std::string label; int num_line; bool full;
				Util::MultilineGen(m_eresources_global_list_right->m_list[i]->m_name,label,num_line, 12, 3, full,1);
				current_label->setLabel(label);
				current_label->setHandler(this);
				if(m_eresources_global_list_right->GetOver() == i){
					current_label->setSize(osg::Vec2f(95.0f, 95.0f));
					current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
					current_label->setFontSize(14);
				}
				else{
					current_label->setSize(osg::Vec2f(80.0f, 80.0f));
					current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
					current_label->setBorderColor(osg::Vec4f(0,0,0,0));
					current_label->setFontSize(12);
				}	
				Red = current_category.Red/255.0;
				Green = current_category.Green/255.0;
				Blue = current_category.Blue/255.0;
				if(m_eresources_global_list_right->GetSelect() == i){	
					current_label->setColor(osg::Vec4f(Red,Green,Blue,1.0f));
					current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
					current_label->setBorderColor(osg::Vec4f(1,1,1,1));
				}
				else{
					current_label->setColor(osg::Vec4f(Red,Green,Blue,0.5f));
					current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
				}
				
				m_widget_multi_list->addWidget(current_label);

				//if the current resource is auto-reposition enabled, also display an icon on the top left corner
				if(m_eresources_global_list_right->m_list[i]->m_reposition == 0)//auto reposition on
				{
					osg::ref_ptr<VSLabel> reposition_marker;
					reposition_marker = new VSLabel("");
					reposition_marker->setFontSize(10);
					reposition_marker->setFontColor(osg::Vec4f(1,1,1,1));
					reposition_marker->setColor(osg::Vec4f(1,1,1,1));
					reposition_marker->setBorderColor(osg::Vec4f(0,0,0,0));
					if(m_eresources_global_list_right->GetOver() == i){
						reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
						reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 82.0f)));

					}
					else{
						reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
						reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 68.0f)));

					}
					m_widget_multi_list->addWidgetToPrev(reposition_marker);//must be add after the widget is added
				}

				m_rightbar_global_offset = m_widget_multi_list->m_subcanvas->getContentOffset();
				m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_global_offset);
			}//end for
			break;
		case LIST_SEARCH:
			{
			list_size = m_eresources_searched_list->m_widgets.size();
			std::string search_string = m_widget_search_input->getLabel();
			if(!list_size&&search_string != "Click to Input for Search"){
				
				current_label = new VSLabel("No Match!");
				current_label->setHandler(this);
				current_label->setSize(osg::Vec2f(m_widget_multi_list->getSize().x() - 100, 90.0f));
				current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
				current_label->setFontSize(36);
				m_widget_multi_list->addWidget(current_label);
			}
			for (i = 0; i != list_size; i++){
				current_label = m_eresources_searched_list->m_widgets[i];
				current_category = m_eresources_searched_list->m_list[i]->m_category;
				std::string label; int num_line; bool full;
				Util::MultilineGen(m_eresources_searched_list->m_list[i]->m_name,label,num_line, 12, 3, full,1);
				current_label->setLabel(label);
				current_label->setHandler(this);
				if(m_eresources_searched_list->GetOver() == i){
					current_label->setSize(osg::Vec2f(95.0f, 95.0f));
					current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
					current_label->setFontSize(14);
				}
				else{
					current_label->setSize(osg::Vec2f(80.0f, 80.0f));
					current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
					current_label->setFontSize(12);
					//set color
				}	
				Red = current_category.Red/255.0;
				Green = current_category.Green/255.0;
				Blue = current_category.Blue/255.0;
				if(m_eresources_searched_list->GetSelect() == i){	
					current_label->setColor(osg::Vec4f(Red,Green,Blue,1.0f));
					current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
					current_label->setBorderColor(osg::Vec4f(1,1,1,1));
				}
				else{
					current_label->setColor(osg::Vec4f(Red,Green,Blue,0.5f));
					current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
					current_label->setBorderColor(osg::Vec4f(0,0,0,0));
				}
				m_widget_multi_list->addWidget(current_label);
				//if the current resource is auto-reposition enabled, also display an icon on the top left corner
				if(m_eresources_searched_list->m_list[i]->m_reposition == 0)//auto reposition on
				{
					osg::ref_ptr<VSLabel> reposition_marker;
					reposition_marker = new VSLabel("");
					reposition_marker->setFontSize(10);
					reposition_marker->setFontColor(osg::Vec4f(1,1,1,1));
					reposition_marker->setColor(osg::Vec4f(1,1,1,1));
					reposition_marker->setBorderColor(osg::Vec4f(0,0,0,0));
					if(m_eresources_searched_list->GetOver() == i){
						reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
						reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 82.0f)));

					}
					else{
						reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
						reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 68.0f)));

					}
					m_widget_multi_list->addWidgetToPrev(reposition_marker);//must be add after the widget is added
				}

				m_rightbar_search_offset = m_widget_multi_list->m_subcanvas->getContentOffset();
				m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_search_offset);
			}//end for
			}
			break;
		case LIST_CATEGORY:
			if (m_category_list.m_expand == false){
				list_size = m_category_list.m_category_vector.size();
				for (i = 0; i != list_size; i++){
					current_label = m_category_list.m_widgets[i];
					current_label->setHandler(this);
					//current_label->setLabel(m_eresources_local_list_left->m_list[i]->m_filetype);
					std::string number_of_files;
					std::stringstream l_out;
					l_out << m_category_list.m_eresources_list_vector[i]->m_list.size();
					number_of_files = l_out.str();

					//extract first two words
					std::string category_name = m_category_list.m_category_vector[i].Name;
					current_label->setLabel(category_name + "\n\n       (" + number_of_files + ")" );

					if(m_category_list.GetOver() == i){
						current_label->setSize(osg::Vec2f(136.0f, 102.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
						current_label->setFontSize(16);
					}
					else{
						current_label->setSize(osg::Vec2f(120.0f, 90.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
						current_label->setFontSize(14);
					}
					//set color
					Red = m_category_list.m_category_vector[i].Red/255.0;
					Green = m_category_list.m_category_vector[i].Green/255.0;
					Blue = m_category_list.m_category_vector[i].Blue/255.0;
					
					if(m_category_list.GetSelect() == i){	
						current_label->setColor(osg::Vec4f(Red,Green,Blue,1.0f));
						current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
						current_label->setBorderColor(osg::Vec4f(1,1,1,1));
					}
					else{
						current_label->setColor(osg::Vec4f(Red,Green,Blue,0.5f));
						current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
						current_label->setBorderColor(osg::Vec4f(0,0,0,0));
					}
					m_widget_multi_list->addWidget(current_label);
					m_rightbar_category_offset = m_widget_multi_list->m_subcanvas->getContentOffset();
					m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_category_offset);
				}//end for
			}
			else{//expand = true
				if(m_category_list.GetOver() < 0)//nothing selected, return
					return;
				if(m_category_list.GetOver() >= (int signed)m_category_list.m_eresources_list_vector.size()){
					m_category_list.SetOver(-1);
					m_category_list.m_expand = false; 
					return;
				}
				list_size = m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_list.size();
				for (i = 0; i != list_size; i++){
					current_category = m_category_list.m_category_vector[m_category_list.GetOver()];
					current_label = m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_widgets[i];
					std::string label; int num_line; bool full;
					Util::MultilineGen(m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_list[i]->m_name,label,num_line, 12, 3, full,1);
					current_label->setLabel(label);
					current_label->setHandler(this);
					if(m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->GetOver() == i){
						current_label->setSize(osg::Vec2f(95.0f, 95.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
						current_label->setFontSize(14);
					}
					else{
						current_label->setSize(osg::Vec2f(80.0f, 80.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
						current_label->setFontSize(12);
					}
					Red = current_category.Red/255.0;
					Green = current_category.Green/255.0;
					Blue = current_category.Blue/255.0;

					if(m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->GetSelect() == i){	
						current_label->setColor(osg::Vec4f(Red,Green,Blue,1.0f));
						current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
						current_label->setBorderColor(osg::Vec4f(1,1,1,1));
					}
					else{
						current_label->setColor(osg::Vec4f(Red,Green,Blue,0.5f));
						current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
						current_label->setBorderColor(osg::Vec4f(0,0,0,0));
					}
					m_widget_multi_list->addWidget(current_label);

					//if the current resource is auto-reposition enabled, also display an icon on the top left corner
					if(m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_list[i]->m_reposition == 0)//auto reposition on
					{
						osg::ref_ptr<VSLabel> reposition_marker;
						reposition_marker = new VSLabel("");
						reposition_marker->setFontSize(10);
						reposition_marker->setFontColor(osg::Vec4f(1,1,1,1));
						reposition_marker->setColor(osg::Vec4f(1,1,1,1));
						reposition_marker->setBorderColor(osg::Vec4f(0,0,0,0));
						if(m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->GetOver() == i){
							reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
							reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 82.0f)));

						}
						else{
							reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
							reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 68.0f)));

						}
						m_widget_multi_list->addWidgetToPrev(reposition_marker);//must be add after the widget is added
					}

					temp_offset = m_widget_multi_list->m_subcanvas->getContentOffset();
					m_widget_multi_list->m_subcanvas->setContentOffset(temp_offset);
				}
				
			}
		break;
		case LIST_TITLE:
			if (m_title_list.m_expand == false){
				list_size = m_title_list.m_title_vector.size();
				for (i = 0; i != list_size; i++){
					current_label = m_title_list.m_widgets[i];
					current_label->setHandler(this);
					//current_label->setLabel(m_eresources_local_list_left->m_list[i]->m_filetype);
					current_label->setFontSize(36);
					std::string number_of_files;
					std::stringstream l_out;
					l_out << m_title_list.m_eresources_list_vector[i]->m_list.size();
					number_of_files = l_out.str();
					std::string label_content;
					label_content = m_title_list.m_title_vector[i];
					label_content += "(" + number_of_files + ")";
					current_label->setLabel(label_content);

					if(m_title_list.GetOver() == i){
						current_label->setSize(osg::Vec2f(136.0f, 102.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
					}
					else{
						current_label->setSize(osg::Vec2f(120.0f, 90.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
					}

					
					if(m_title_list.GetSelect() == i){	
						current_label->setColor(osg::Vec4f(1,1,1,1.0f));
						current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
						current_label->setBorderColor(osg::Vec4f(1,1,1,1));
					}
					else{
						current_label->setColor(osg::Vec4f(0,0,0,0.3f));
						current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
						current_label->setBorderColor(osg::Vec4f(0,0,0,0));
					}

					m_widget_multi_list->addWidget(current_label);
					m_rightbar_title_offset = m_widget_multi_list->m_subcanvas->getContentOffset();
					m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_title_offset);
				}//end for
			}
			else{//expand = true
				if(m_title_list.GetOver() < 0)//nothing selected, return
					return;
				if(m_title_list.GetOver() >= (int signed)m_title_list.m_eresources_list_vector.size()){
					m_title_list.SetOver(-1);
					m_title_list.m_expand = false;
					return;
				}
				list_size = m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_list.size();
				for (i = 0; i != list_size; i++){
					current_category = m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_list[i]->m_category;
					current_label = m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_widgets[i];
					std::string label; int num_line; bool full;
					Util::MultilineGen(m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_list[i]->m_name,label,num_line, 12, 3, full,1);
					current_label->setLabel(label);
					current_label->setHandler(this);
					if(m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->GetOver() == i){
						current_label->setSize(osg::Vec2f(95.0f, 95.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
						current_label->setFontSize(14);
					}
					else{
						current_label->setSize(osg::Vec2f(80.0f, 80.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
						current_label->setFontSize(12);
					}
					Red = current_category.Red/255.0;
					Green = current_category.Green/255.0;
					Blue = current_category.Blue/255.0;

					if(m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->GetSelect() == i){	
						current_label->setColor(osg::Vec4f(Red,Green,Blue,1.0f));
						current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
						current_label->setBorderColor(osg::Vec4f(1,1,1,1));
					}
					else{
						current_label->setColor(osg::Vec4f(Red,Green,Blue,0.5f));
						current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
						current_label->setBorderColor(osg::Vec4f(0,0,0,0));
					}
					m_widget_multi_list->addWidget(current_label);
					//if the current resource is auto-reposition enabled, also display an icon on the top left corner
					if(m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_list[i]->m_reposition == 0)//auto reposition on
					{
						osg::ref_ptr<VSLabel> reposition_marker;
						reposition_marker = new VSLabel("");
						reposition_marker->setFontSize(10);
						reposition_marker->setFontColor(osg::Vec4f(1,1,1,1));
						reposition_marker->setColor(osg::Vec4f(1,1,1,1));
						reposition_marker->setBorderColor(osg::Vec4f(0,0,0,0));
						if(m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->GetOver() == i){
							reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
							reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 82.0f)));

						}
						else{
							reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
							reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 68.0f)));

						}
						m_widget_multi_list->addWidgetToPrev(reposition_marker);//must be add after the widget is added
					}

					temp_offset = m_widget_multi_list->m_subcanvas->getContentOffset();
					m_widget_multi_list->m_subcanvas->setContentOffset(temp_offset);
				}
			}
		break;
		case LIST_TYPE:
			if (m_type_list.m_expand == false){
				list_size = m_type_list.m_type_vector.size();
				for (i = 0; i != list_size; i++){
					current_label = m_type_list.m_widgets[i];
					current_label->setHandler(this);
					current_label->setFontSize(24);
					std::string number_of_files;
					std::stringstream l_out;
					l_out << m_type_list.m_eresources_list_vector[i]->m_list.size();
					number_of_files = l_out.str();
					std::string label_content;
					label_content = m_type_list.m_type_vector[i];
					label_content += "(" + number_of_files + ")";
					current_label->setLabel(label_content);

					if(m_type_list.GetOver() == i){
						current_label->setSize(osg::Vec2f(136.0f, 102.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
					}
					else{
						current_label->setSize(osg::Vec2f(120.0f, 90.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
					}

					
					if(m_type_list.GetSelect() == i){	
						current_label->setColor(osg::Vec4f(1,1,1,1.0f));
						current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
						current_label->setBorderColor(osg::Vec4f(1,1,1,1));
					}
					else{
						current_label->setColor(osg::Vec4f(0,0,0,0.3f));
						current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
						current_label->setBorderColor(osg::Vec4f(0,0,0,0));
					}

					m_widget_multi_list->addWidget(current_label);
					m_rightbar_type_offset = m_widget_multi_list->m_subcanvas->getContentOffset();
					m_widget_multi_list->m_subcanvas->setContentOffset(m_rightbar_type_offset);
				}//end for
			}
			else{//expand = true
				if(m_type_list.GetOver() < 0)//nothing selected, return
					return;
				if(m_type_list.GetOver() >= (int signed)m_type_list.m_eresources_list_vector.size()){
					m_type_list.SetOver(-1);
					m_type_list.m_expand = false;
					return;
				}
				list_size = m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_list.size();
				for (i = 0; i != list_size; i++){
					current_category = m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_list[i]->m_category;
					current_label = m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_widgets[i];
					std::string label; int num_line; bool full;
					Util::MultilineGen(m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_list[i]->m_name,label,num_line, 12, 3, full,1);
					current_label->setLabel(label);
					current_label->setHandler(this);
					if(m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->GetOver() == i){
						current_label->setSize(osg::Vec2f(95.0f, 95.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
						current_label->setFontSize(14);
					}
					else{
						current_label->setSize(osg::Vec2f(80.0f, 80.0f));
						current_label->setPosition(osg::Vec2f(0.0f, 0.0f));
						current_label->setFontSize(12);
					}
					Red = current_category.Red/255.0;
					Green = current_category.Green/255.0;
					Blue = current_category.Blue/255.0;

					if(m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->GetSelect() == i){	
						current_label->setColor(osg::Vec4f(Red,Green,Blue,1.0f));
						current_label->setFontColor(osg::Vec4f(0,0,0,1.0f));
						current_label->setBorderColor(osg::Vec4f(1,1,1,1));
					}
					else{
						current_label->setColor(osg::Vec4f(Red,Green,Blue,0.5f));
						current_label->setFontColor(osg::Vec4f(1,1,1,1.0f));
						current_label->setBorderColor(osg::Vec4f(0,0,0,0));
					}
					m_widget_multi_list->addWidget(current_label);
					//if the current resource is auto-reposition enabled, also display an icon on the top left corner
					if(m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_list[i]->m_reposition == 0)//auto reposition on
					{
						osg::ref_ptr<VSLabel> reposition_marker;
						reposition_marker = new VSLabel("");
						reposition_marker->setFontSize(10);
						reposition_marker->setFontColor(osg::Vec4f(1,1,1,1));
						reposition_marker->setColor(osg::Vec4f(1,1,1,1));
						reposition_marker->setBorderColor(osg::Vec4f(0,0,0,0));
						if(m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->GetOver() == i){
							reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
							reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 82.0f)));

						}
						else{
							reposition_marker->setSize(osg::Vec2f(15.0f, 15.0f));
							reposition_marker->setPosition(osg::Vec2f(osg::Vec2f(-3.0f, 68.0f)));

						}
						m_widget_multi_list->addWidgetToPrev(reposition_marker);//must be add after the widget is added
					}
					temp_offset = m_widget_multi_list->m_subcanvas->getContentOffset();
					m_widget_multi_list->m_subcanvas->setContentOffset(temp_offset);
				}
			}
		break;
	}//end switch
}

void EResourcesListEditor::updateRangedList(){

	int slider_year;
	if(g_vsimapp->getTimeSwitch())
		slider_year = g_vsimapp->getTimeSwitch()->getValue();
	else
		slider_year = 0;

	//first need to free things in the toList
	EResourcesNodeVector::iterator ERiter;
	WidgetVector::iterator Witer;

	m_eresources_ranged_list->m_list.clear();
    m_eresources_ranged_list->m_widgets.clear();

	m_eresources_ranged_list->SetSelect(-1);
	m_eresources_ranged_list->SetOver(-1);
	
	for (ERiter = m_eresources_central_list->m_list.begin(), Witer = m_eresources_central_list->m_widgets.begin(); ERiter != m_eresources_central_list->m_list.end(); ERiter++,Witer++){
		osg::ref_ptr<EResourcesNode> fromNode;
		fromNode = *ERiter;
		if(fromNode->m_min_year <= slider_year && fromNode->m_max_year >= slider_year){//if it is a local node
			m_eresources_ranged_list->m_list.push_back(fromNode);//shadow copy
			osg::ref_ptr<VSLabel>  fromLabel;
			std::string label = (*Witer)->getLabel();
			fromLabel = new VSLabel(label);//deep copy
			m_eresources_ranged_list->m_widgets.push_back(fromLabel);
		}
	}
}

void EResourcesListEditor::updateEndLists()
{
	EResourcesInfoCard::Instance()->HideInfoCard();

	this->updateRangedList();//range list is based on time slider

	m_filter.applyFilter(m_eresources_ranged_list, m_eresources_filted_list);//the filter's list is updated

	extractLocalGlobal(m_eresources_local_list_right, m_eresources_ranged_list, 1);

	extractLocalGlobal(m_eresources_global_list_right, m_eresources_ranged_list, 0 );

	m_category_list.UpdateContent(m_eresources_ranged_list);

	m_title_list.UpdateContent(m_eresources_ranged_list);

	m_type_list.UpdateContent(m_eresources_ranged_list);

	extractLocalGlobal(m_eresources_local_list_left, m_eresources_filted_list, 1);
	
	extractLocalGlobal(m_eresources_global_list_left, m_eresources_filted_list, 0 );

	UpdateSearch();

	drawMultiLeft();
	drawMultiRight();

}

void EResourcesListEditor::updateFilteredLists()
{

	m_filter.applyFilter(m_eresources_central_list, m_eresources_filted_list);//the filter's list is updated

	extractLocalGlobal(m_eresources_local_list_left, m_eresources_filted_list, 1);
	
	extractLocalGlobal(m_eresources_global_list_left, m_eresources_filted_list, 0 );

	drawMultiLeft();

}


/*
void EResourcesListEditor::updateFromLocalGlobalRight(){

	m_eresources_central_list->m_list.clear();
	m_eresources_central_list->m_widgets.clear();

	int num_node;
	VSLabel *templabel;
	VSLabel *newlabel;
	EResourcesNode * tempnode;

	num_node = m_eresources_local_list_right->m_list.size();
	for(int iter = 0; iter < num_node; iter++){
		tempnode = m_eresources_local_list_right->m_list[iter];
		templabel = m_eresources_local_list_right->m_widgets[iter];

		m_eresources_central_list->m_list.push_back(tempnode);
		newlabel = new VSLabel(templabel->getLabel());
		m_eresources_central_list->m_widgets.push_back(newlabel);
	}

	num_node = m_eresources_global_list_right->m_list.size();
	for(int iter = 0; iter < num_node; iter++){
		tempnode = m_eresources_global_list_right->m_list[iter];
		templabel = m_eresources_global_list_right->m_widgets[iter];

		m_eresources_central_list->m_list.push_back(tempnode);
		newlabel = new VSLabel(templabel->getLabel());
		m_eresources_central_list->m_widgets.push_back(newlabel);
	}

}

void EResourcesListEditor::updateFromLocalGlobalLeft(){

	m_eresources_central_list->m_list.clear();
	m_eresources_central_list->m_widgets.clear();

	int num_node;
	VSLabel *templabel;
	VSLabel *newlabel;
	EResourcesNode * tempnode;

	num_node = m_eresources_local_list_left->m_list.size();
	for(int iter = 0; iter < num_node; iter++){
		tempnode = m_eresources_local_list_left->m_list[iter];
		templabel = m_eresources_local_list_left->m_widgets[iter];

		m_eresources_central_list->m_list.push_back(tempnode);
		newlabel = new VSLabel(templabel->getLabel());
		m_eresources_central_list->m_widgets.push_back(newlabel);
	}

	num_node = m_eresources_global_list_left->m_list.size();
	for(int iter = 0; iter < num_node; iter++){
		tempnode = m_eresources_global_list_left->m_list[iter];
		templabel = m_eresources_global_list_left->m_widgets[iter];

		m_eresources_central_list->m_list.push_back(tempnode);
		newlabel = new VSLabel(templabel->getLabel());
		m_eresources_central_list->m_widgets.push_back(newlabel);
	}

}
*/

void EResourcesListEditor::extractLocalGlobal(osg::ref_ptr<EResourcesList> toList, osg::ref_ptr<EResourcesList> fromList, int local_global){
	
	//first need to free things in the toList
	EResourcesNodeVector::iterator ERiter;
	WidgetVector::iterator Witer;

	toList->m_list.clear();
    toList->m_widgets.clear();

	toList->SetSelect(-1);
	toList->SetOver(-1);
	
	for (ERiter = fromList->m_list.begin(), Witer = fromList->m_widgets.begin(); ERiter != fromList->m_list.end(); ERiter++,Witer++){
		osg::ref_ptr<EResourcesNode> fromNode;
		fromNode = *ERiter;
		if(fromNode->m_global == local_global){//if it is a local node
			toList->m_list.push_back(fromNode);//shadow copy
			osg::ref_ptr<VSLabel>  fromLabel;
			std::string label = (*Witer)->getLabel();
			fromLabel = new VSLabel(label);//deep copy
			toList->m_widgets.push_back(fromLabel);
		}
	}
	
}

void EResourcesListEditor::copyList(osg::ref_ptr<EResourcesList> toList, osg::ref_ptr<EResourcesList> fromList){
	
	//first need to free things in the toList
	EResourcesNodeVector::iterator ERiter;
	WidgetVector::iterator Witer;

	toList->m_list.clear();
    toList->m_widgets.clear();

	toList->SetSelect(-1);
	toList->SetOver(-1);
	
	for (ERiter = fromList->m_list.begin(); ERiter != fromList->m_list.end(); ERiter++){
		osg::ref_ptr<EResourcesNode> fromNode;
		fromNode = *ERiter;
		toList->m_list.push_back(fromNode);
	}

	for (Witer = fromList->m_widgets.begin(); Witer != fromList->m_widgets.end(); Witer++){
		osg::ref_ptr<VSLabel>  fromLabel;
		fromLabel = new VSLabel((*Witer)->getLabel());//deep copy
		toList->m_widgets.push_back(fromLabel);
	}

	
}


void EResourcesListEditor::unhighlightButton(int button_type){
	//set the color of previous back to normal
	switch(button_type){
		case BUTTON_GLOBAL:
			m_widget_global->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_LOCAL:
			m_widget_local->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_TITLE:
			m_widget_title->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_CATEGORY:
			m_widget_category->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_TYPE:
			m_widget_filetype->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_LIST:
			m_widget_list->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_FILTER:
			m_widget_filter->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_ADD:
			m_widget_add_left->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_DELETE:
			m_widget_remove_left->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_EDIT:
			m_widget_edit_left->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_ADD_2:
			m_widget_add_right->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_DELETE_2:
			m_widget_remove_right->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_EDIT_2:
			m_widget_edit_right->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_SEARCH_INPUT:
			m_widget_search_input->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_SEARCH_LABEL:
			m_widget_search_label->setColor(g_settings.m_widget_bg_color);
			break;
		case BUTTON_SEARCH_CANCEL:
			m_widget_search_cancel->setColor(g_settings.m_widget_bg_color);
			break;
	}
}

void EResourcesListEditor::unhighlightAll(){
			unhighlightButton(BUTTON_LOCAL);
			unhighlightButton(BUTTON_TITLE);
			unhighlightButton(BUTTON_GLOBAL);
			unhighlightButton(BUTTON_CATEGORY);
			unhighlightButton(BUTTON_TYPE);
			unhighlightButton(BUTTON_SEARCH_LABEL);
}
void EResourcesListEditor::highlightButton(int button_type){

	switch(button_type){
		case BUTTON_GLOBAL:
			unhighlightAll();
			m_widget_global->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_LOCAL:
			unhighlightAll();
			m_widget_local->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_TITLE:
			unhighlightAll();
			m_widget_title->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_CATEGORY:
			unhighlightAll();
			m_widget_category->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_TYPE:
			unhighlightAll();
			m_widget_filetype->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_LIST:
			m_widget_list->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_FILTER:
			m_widget_filter->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_ADD:
			m_widget_add_left->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_DELETE:
 			m_widget_remove_left->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_EDIT:
			m_widget_edit_left->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_ADD_2:
			m_widget_add_right->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_DELETE_2:
			m_widget_remove_right->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_EDIT_2:
			m_widget_edit_right->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_SEARCH_INPUT:
			m_widget_search_input->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_SEARCH_LABEL:
			unhighlightAll();
			m_widget_search_label->setColor(g_settings.m_widget_selected_bg_color);
			break;
		case BUTTON_SEARCH_CANCEL:
			m_widget_search_cancel->setColor(g_settings.m_widget_selected_bg_color);
			break;	
	}
}


void EResourcesListEditor::exportEResources(){

	if(m_eresources_central_list->getSize() == 0){
		MessageDialog md;
        std::stringstream ss;
        ss << "Empty List, Aborting Save";
        md.show(ss.str());
		return;

	}

	GenericFileDialog gfd;
    gfd.init("Embedded Resources files (*.ere)|*.ere|All files (*.*)|*.*", VSIM_DIALOG_SAVE);
    bool result = gfd.show();
    if (result == false)
        return;
    std::string filename = gfd.getPathName();
	filename = Util::addExtensionIfNotExist(filename, "ere");

    saveEResources(filename);

}

void EResourcesListEditor::importEResources(){

	GenericFileDialog gfd;
    gfd.init("Embedded Resources files (*.ere)|*.ere|All files (*.*)|*.*", VSIM_DIALOG_LOAD);
    bool result = gfd.show();
    if (result == false)
        return;
    std::string filename = gfd.getPathName();
    filename = Util::addExtensionIfNotExist(filename, "ere");

	loadEResources(filename);
}


void EResourcesListEditor::importEResources(const std::string& filename){

	std::string filename_withextension = Util::addExtensionIfNotExist(filename, "ere");

	loadEResources(filename_withextension);
}

#ifdef FILE_SAVE_ASCII
    static const char* s_readertype = "osgt";
#else
    static const char* s_readertype = "osgb";
#endif

void EResourcesListEditor::addEResourcesToNode(osg::Node* node){

	if(m_eresources_central_list->getSize() == 0){
		//empty list no need to save
		return;

	}

	m_eresources_central_list->setLock(true);
	if(node->asGroup()->containsNode(m_eresources_central_list)){	
	}else{
		node->asGroup()->addChild(m_eresources_central_list);
	}

}

void EResourcesListEditor::extractEResourcesFromNode(osg::Node* node){
	
	m_eresources_central_list->Clear();

	//clear the previous effect, incase the loaded model have no eresources
	m_widget_add_left->show();
	m_widget_remove_left->show();
	m_widget_edit_left->show();
	m_widget_add_right->show();
	m_widget_remove_right->show();
	m_widget_edit_right->show();
	m_widget_locked_left->hide();
	m_widget_locked_right->hide();


	EResourcesList* temp_list;
	unsigned int NumChildren = node->asGroup()->getNumChildren();
    for (unsigned int i = 0; i < NumChildren; i++)
    {
        osg::Node* c = node->asGroup()->getChild(i);
        temp_list = dynamic_cast<EResourcesList*>(c);
        if (temp_list)
        {
			m_eresources_central_list = temp_list;
			if(m_eresources_central_list->getLock())
			{
				//locked
				m_widget_add_left->hide();
				m_widget_remove_left->hide();
				m_widget_edit_left->hide();
				m_widget_add_right->hide();
				m_widget_remove_right->hide();
				m_widget_edit_right->hide();
				m_widget_locked_left->show();
				m_widget_locked_right->show();

			}
			break;
		}
    }	
	updateEndLists();
	setWidgets();
}

void EResourcesListEditor::addEResourcesFromNode(osg::Node* node){

	//clear the previous effect, incase the loaded model have no eresources
	m_widget_add_left->show();
	m_widget_remove_left->show();
	m_widget_edit_left->show();
	m_widget_add_right->show();
	m_widget_remove_right->show();
	m_widget_edit_right->show();
	m_widget_locked_left->hide();
	m_widget_locked_right->hide();

	EResourcesList* temp_list;
	unsigned int NumChildren = node->asGroup()->getNumChildren();
    for (unsigned int i = 0; i < NumChildren; i++)
    {
        osg::Node* c = node->asGroup()->getChild(i);
        temp_list = dynamic_cast<EResourcesList*>(c);
        if (temp_list)
        {
			if(temp_list->getLock())
			{
				m_eresources_central_list->setLock(true);
				//locked
				m_widget_add_left->hide();
				m_widget_remove_left->hide();
				m_widget_edit_left->hide();
				m_widget_add_right->hide();
				m_widget_remove_right->hide();
				m_widget_edit_right->hide();
				m_widget_locked_left->show();
				m_widget_locked_right->show();
			}
			m_eresources_central_list->AddList(temp_list);
			break;
		}
    }	
	updateEndLists();
	setWidgets();
}

void EResourcesListEditor::saveEResources(std::string filename){

    std::ofstream ofs;
    ofs.open(filename.c_str(), std::ios::binary);
    if (!ofs.good())
    {
        MessageDialog md;
        std::stringstream ss;
        ss << "Error opening file " << filename << " for write.";
        md.show(ss.str());
        return;
    }

	if(!(m_eresources_central_list->getLock())){
		//unlocked, need to ask if want locked in the future
		//apply the choice
		wxWindow* parent = wxGetApp().GetTopWindow();
		int answer = wxMessageBox("Locking the file will render it read-only. If you wish to be able to edit it again, you may want to save a non-locked copy. Locking a file is NOT reversible.", "Lock the file?", wxYES_NO | wxNO_DEFAULT | wxSTAY_ON_TOP, parent);

		if (answer == wxYES){
			parent = wxGetApp().GetTopWindow();
			int answer2 = wxMessageBox("Are you sure you wish to lock the file? This cannot be undone. Choose 'Cancel' to save without locking.", "Are you sure?", wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxSTAY_ON_TOP, parent);
			if (answer2 == wxOK)
				m_eresources_central_list->setLock(true);
		}
	}

    osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension(s_readertype);
    assert(rw);
#ifdef FILE_SAVE_ASCII
    osgDB::ReaderWriter::WriteResult wresult = rw->writeNode(*m_eresources_central_list, ofs, new osgDB::Options("Ascii"));
#else
    osgDB::ReaderWriter::WriteResult wresult = rw->writeNode(*m_eresources_central_list, ofs);
#endif
    ofs.close();

    if (!wresult.success())
    {
        MessageDialog md;
        std::stringstream ss;
        ss << "Error saving embedded resources: " << filename;
        md.show(ss.str());
    }

	return;

}

void EResourcesListEditor::loadEResources(std::string filename){
    std::ifstream ifs;
    ifs.open(filename.c_str(), std::ios::binary);
    if (!ifs.good())
    {
        MessageDialog md;
        std::stringstream ss;
        ss << "Error opening file " << filename << " for read.";
        md.show(ss.str());
        return; 
    }

    osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension(s_readertype);
    assert(rw);
    osgDB::ReaderWriter::ReadResult rresult = rw->readNode(ifs);
    ifs.close();

	m_eresources_central_list->Clear();

    if (rresult.success())
    {
        m_eresources_central_list = dynamic_cast<EResourcesList*>(rresult.takeNode());
    }
    if (!m_eresources_central_list.get())
    {
        MessageDialog md;
        std::stringstream ss;
        ss << "Error loading embedded resources: " << filename;
        md.show(ss.str());
        return;
    }

	updateEndLists();
	if(m_eresources_central_list->getLock())
	{
		//locked
		m_widget_add_left->hide();
		m_widget_remove_left->hide();
		m_widget_edit_left->hide();
		m_widget_add_right->hide();
		m_widget_remove_right->hide();
		m_widget_edit_right->hide();
		m_widget_locked_left->show();
		m_widget_locked_right->show();

	}
	else{
		m_widget_add_left->show();
		m_widget_remove_left->show();
		m_widget_edit_left->show();
		m_widget_add_right->show();
		m_widget_remove_right->show();
		m_widget_edit_right->show();
		m_widget_locked_left->hide();
		m_widget_locked_right->hide();
	}
	setWidgets();
}

void EResourcesListEditor::showEResources(bool launch){
	setHoldGravity(true);
	osg::ref_ptr<EResourcesNode> showNode;
	int selection;
	switch(m_list_over){
		case LIST_CATEGORY:
			selection = m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->GetSelect();
			showNode = m_category_list.m_eresources_list_vector[m_category_list.GetOver()]->m_list[selection];
			break;
		case LIST_TYPE:
			selection = m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->GetSelect();
			showNode = m_type_list.m_eresources_list_vector[m_type_list.GetOver()]->m_list[selection];
			break;
		case LIST_TITLE:
			selection = m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->GetSelect();
			showNode = m_title_list.m_eresources_list_vector[m_title_list.GetOver()]->m_list[selection];
			break;
		case LIST_GLOBAL_RIGHT:
			selection = m_eresources_global_list_right->GetSelect();
			showNode = m_eresources_global_list_right->m_list[selection];
			break;
		case LIST_LOCAL_RIGHT:
			selection = m_eresources_local_list_right->GetSelect();
			showNode = m_eresources_local_list_right->m_list[selection];
			break;
		case LIST_GLOBAL_LEFT:
			selection = m_eresources_global_list_left->GetSelect();
			showNode = m_eresources_global_list_left->m_list[selection];
			break;
		case LIST_LOCAL_LEFT:
			selection = m_eresources_local_list_left->GetSelect();
			showNode = m_eresources_local_list_left->m_list[selection];
			break;
		case LIST_SEARCH:
			selection = m_eresources_searched_list->GetSelect();
			showNode = m_eresources_searched_list->m_list[selection];
			break;
	}//determine which node to show

	if(showNode->m_reposition == 0){//auto reposition on
		m_transitioner->TurnOn(showNode);
	}
	
	//always show the expanded infocard on screen
	EResourcesInfoCard::Instance()->ShowExpandedInfoCard(showNode);

	if(launch){//if launch on
		if(showNode->m_ertype == 1){
			//open the link using default browser
			wxLaunchDefaultBrowser(showNode->m_filepath);
		}
		else if(showNode->m_ertype == 0){
			//open the file using default program
			wxString command;
			wxMimeTypesManager manager;
			wxFileType * filetype = manager.GetFileTypeFromExtension(showNode->m_filetype);
			if(!filetype){
				MessageDialog warning;
				warning.show("File type not recognized!", MESSAGE_DIALOG_OK);
				wxLaunchDefaultBrowser(showNode->m_filepath);
				return;
			}
			command = filetype->GetOpenCommand(showNode->m_filepath);
			processCommand(command);
			if(command.size())
				wxExecute(command);
			else{
				MessageDialog warning;
				warning.show("Can not open the file!", MESSAGE_DIALOG_OK);
				wxLaunchDefaultBrowser(showNode->m_filepath);
				return;
			}
		}
	}
}

void EResourcesListEditor::processCommand(wxString& command){
	wxString pattern = "rundll32.exe";
	size_t found;
	found = command.find(pattern);
	if(found!=std::string::npos){
		//delete the double quote if exist
		//first check whether double quote exist 
		wxString::iterator iter;
		iter = command.end();
		iter--;
		if(*iter == 34){
		//double quote occur, delete
			command.erase(iter);//delete the last one
			//search for the first one
			iter = command.end();
			iter--;
			while(*iter != 34)
				iter--;
			command.erase(iter);
		}
	}
}

void EResourcesListEditor::showBar()
{ 
   if (m_status == WIDGET_CLOSED_LEFT || m_status == WIDGET_CLOSED_RIGHT)     
		handleMouseLeftClick(m_widget_hide_left);
   if (m_status == WIDGET_FULL_LEFT){
		handleMouseLeftClick(m_widget_hide_left);
	}
}

void EResourcesListEditor::hideBar()
{
   if (m_status == WIDGET_HALF_LEFT||m_status == WIDGET_HALF_RIGHT)     
		handleMouseLeftClick(m_widget_hide_left);
   if (m_status == WIDGET_FULL_LEFT){
		handleMouseLeftClick(m_widget_hide_left);
		handleMouseLeftClick(m_widget_hide_left);
   } 
}

void EResourcesListEditor::showBar2()
{ 
   
   if (m_status == WIDGET_CLOSED_LEFT || m_status == WIDGET_CLOSED_RIGHT)     
		handleMouseLeftClick(m_widget_hide_left);
   if (m_status == WIDGET_FULL_LEFT){
		handleMouseLeftClick(m_widget_hide_left);
   }

   m_widget_hide_left->show();
   m_widget_hide_right->show();

}

void EResourcesListEditor::hideBar2()
{
   if (m_status == WIDGET_HALF_LEFT||m_status == WIDGET_HALF_RIGHT)     
		handleMouseLeftClick(m_widget_hide_left);
   if (m_status == WIDGET_FULL_LEFT){
		handleMouseLeftClick(m_widget_hide_left);
		handleMouseLeftClick(m_widget_hide_left);
   }
   m_widget_hide_left->hide();
   m_widget_hide_right->hide();

}

void EResourcesListEditor::showhide()
{
	m_widget_hide_left->show();
    m_widget_hide_right->show();
}

bool EResourcesListEditor::IsBarOpen(){
	if(m_status == WIDGET_CLOSED_LEFT || m_status == WIDGET_CLOSED_RIGHT)
		return false;
	else
		return true;
}


float EResourcesListEditor::getBarHeight(){

	switch(m_status){
		case WIDGET_CLOSED_LEFT:
		case WIDGET_CLOSED_RIGHT:
			return 0.0f;
		case WIDGET_HALF_LEFT:
		case WIDGET_HALF_RIGHT:
			return g_settings.m_bar_size_2;
		case WIDGET_FULL_LEFT:
			return g_settings.m_bar_size_2 + 110;
		default:
			return g_settings.m_bar_size_2;
	}

}

void EResourcesListEditor::ajustBarRatioButton(){
	AdjustRatioDialog dlg;
	dlg.SetRatio(m_bar_ratio);
	int result = dlg.ShowModal();
    if (result != wxID_OK)
        return;
	m_bar_ratio = dlg.GetRatio();
	setWidgets();
}

void EResourcesListEditor::setHoldGravity(bool value)
{
    osgGA::CameraManipulator* cm = g_viewer->getCameraManipulator();
    NRTSKeySwitchMatrixManipulator* km = dynamic_cast<NRTSKeySwitchMatrixManipulator*>(cm);
    if (km)
        km->setHoldGravity(value);
}


bool cmpCategory(osg::ref_ptr<EResourcesNode> node){
	return node->m_category.Name == "*******";//marker
}
bool cmpWidget(osg::ref_ptr<VSLabel> node){
	return node->getLabel() == "*******";//marker
}


void EResourcesTransitioner::ApplyTransition(float dt){
	assert(m_on_off);//must be on
	assert(m_node);//must have a node
	assert(dt >= 0);

	if(dt == 0)
		return;

	m_timer += dt;

	if(m_timer > m_transition_time){
		TurnOff();
		return;
	}
	//based on timer, get the ratio t;
	double t0 = m_timer / m_transition_time;
    assert(t0 >= 0.0 && t0 <= 1.0);
    static const double r = sqrt(2.0/3.0);
    static const double d = r - (r * r * r * 0.5);
    t0 = (t0 - 0.5) * 2.0 * r;
    double t = t0 - (t0 * t0 * t0 * 0.5);
    t /= d;
    t *= 0.5;
    t += 0.5;
    assert(t >= -0.0000001 && t <= 1.0000001);
	
    osg::Vec3d pos0 = m_matrix_from.getTrans();
    osg::Vec3d pos1 = m_matrix_to.getTrans();
    osg::Vec3d pos = pos0 * (1.0 - t) + pos1 * t;
    osg::Quat ori0 = m_matrix_from.getRotate();
    osg::Quat ori1 = m_matrix_to.getRotate();
    osg::Quat ori;
    ori.slerp(t, ori0, ori1);
    // Remove banking component
    Util::fixVerticalAxis(ori, osg::Vec3d(0.0, 0.0, 1.0), false);
    osg::Matrixd frame_matrix = osg::Matrixd::rotate(ori) * osg::Matrixd::translate(pos);

	g_viewer->getCameraManipulator()->setByMatrix(frame_matrix);
}

void EResourcesTransitioner::SetTransitionNode(osg::ref_ptr<EResourcesNode> node)
{
	m_node = node;
	m_matrix_from = g_viewer->getCameraManipulator()->getMatrix();
	m_matrix_to = m_node->getViewMatrix();
}