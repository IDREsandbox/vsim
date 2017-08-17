/*
 * EResourcesInfoCard.cpp
 *
 *  Created on: 10 25, 2012
 *      Author: Franklin Fang
 */

#include "test_Prefix.pch"
#include "resources/EResourcesInfoCard.h"
#include "osgNewWidget/WidgetManager.h"
#include "VSimApp.h"
#include "narrative/NarrativeManager.h"
#include "resources/EResourcesManager.h"
#include "menu/Settings.h"
#include "util.h"
static EResourcesInfoCard* s_eresources_infocard = NULL;

EResourcesInfoCard::EResourcesInfoCard(){
	Init();
}

EResourcesInfoCard::~EResourcesInfoCard(){

}


void EResourcesInfoCard::handle(osgNewWidget::Widget* widget, const osgNewWidget::WidgetEvent& ev)
{
	
	switch(ev.type){

	case osgNewWidget::WidgetEvent::MOUSE_PUSH:   
		if(widget == m_expand)
		{
			m_expanded = m_expanded? false:true;//update the status
	
			if(!m_expanded){
				ShowInfoCard(m_er);
			}
			else
				ShowExpandedInfoCard(m_er);
		}
		break;

	default:
		return;
	}//end switch
}

EResourcesInfoCard* EResourcesInfoCard::Instance(){
	if (!s_eresources_infocard)
        s_eresources_infocard = new EResourcesInfoCard();
    return s_eresources_infocard;
}

void EResourcesInfoCard::Init(){

	m_status = INFO_CARD_HIDE;
	m_expanded = false;

	m_canvas = new VSCanvas(osgNewWidget::WIDGET_WITH_GEOMETRY);

	osgNewWidget::Canvas* uiCanvas = osgNewWidget::WidgetManager::instance()->getUIRoot();
	uiCanvas->addWidget(m_canvas);//add the canvas widget to the root canvas

    m_title = new VSLabel("");
	m_canvas->addWidget(m_title.get());//add the hide widget to the root can
	
	m_category = new VSLabel("");
	m_canvas->addWidget(m_category.get());//add the hide widget to the root can
	
	m_decription = new VSLabel("");
	m_canvas->addWidget(m_decription.get());//add the hide widget to the root can

	m_location = new VSLabel("");
	m_location->setPosition(osg::Vec2f(-100,100));
	m_canvas->addWidget(m_location.get());//add the hide widget to the root can

	m_expand = new VSLabel("EXPAND");
	m_expand->setPosition(osg::Vec2f(-100,100));
	m_expand->setHandler(this);
	m_canvas->addWidget(m_expand.get());//add the hide widget to the root can

//	m_author = new VSLabel("");
//	m_canvas->addWidget(m_author.get());//add the hide widget to the root can

	m_canvas->setPosition(osg::Vec2f(-200,-200));

}


void EResourcesInfoCard::ShowInfoCard(osg::ref_ptr<EResourcesNode> ernode){

	m_er = ernode;
	m_expanded = false;
	if(m_status == INFO_CARD_DISPLAY){
		HideInfoCard();
	}

	UpdateInfoCard();
	
	//push the card to display
	osgNewWidget::WidgetAnimator::instance()->add(m_canvas, osgNewWidget::ANIM_PROPERTY_POS_X,
                0.0f, 0.6f, 10, this);

	m_status = INFO_CARD_DISPLAY;
}

void EResourcesInfoCard::ShowExpandedInfoCard(osg::ref_ptr<EResourcesNode> ernode){

	m_er = ernode;
	m_expanded = true;

	if(m_status == INFO_CARD_DISPLAY){
		HideInfoCard();
	}

	UpdateExpandedInfoCard();
	
	//push the card to display
	osgNewWidget::WidgetAnimator::instance()->add(m_canvas, osgNewWidget::ANIM_PROPERTY_POS_X,
                0.0f, 0.6f, 10, this);

	m_status = INFO_CARD_DISPLAY;
}

void EResourcesInfoCard::HideInfoCard(){

	osgNewWidget::WidgetAnimator::instance()->add(m_canvas, osgNewWidget::ANIM_PROPERTY_POS_X,
                0.0f, 0.6f, -400, this);

	m_status = INFO_CARD_HIDE;
}


void EResourcesInfoCard::UpdateInfoCard(){

	assert(m_er != NULL);

	//set labels
	m_category->setLabel(m_er->getCategoryName());
	m_title->setLabel(m_er->getResourceName());
	m_expand->setLabel("MORE");
	std::string location;
	int num_line_location = 0;
	bool line_full = false;
	if(m_er->m_ertype != 2)//not annotation, there is a file location or url
	{
		Util::MultilineGen2(m_er->m_filepath, location, num_line_location, 34, 5, line_full);
		m_location->setLabel(location);
	}

	//multiple line
	std::string description;
	int num_line_description;
	Util::MultilineGen(m_er->m_description, description, num_line_description, 34, 5, line_full);
	m_decription->setLabel(description);

	//set widget size
	osgNewWidget::Canvas* uiCanvas = osgNewWidget::WidgetManager::instance()->getUIRoot();
	osg::Vec2f rsize = uiCanvas->getSize();
	osg::Vec2f canvas_size, canvas_pos;
	
	m_category->setSize(osg::Vec2f(230, 26));
	m_title->setSize(osg::Vec2f(230, 36));
	m_expand->setSize(osg::Vec2f(52,20));
	m_decription->setSize(osg::Vec2f(230, num_line_description*25));

	if(m_er->m_ertype != 2)//not annotation
  		m_location->setSize(osg::Vec2f(230, num_line_location*16));
	
	canvas_size.x() = 250;
	canvas_size.y() = m_title->getSize().y() + m_category->getSize().y() + 20 + m_decription->getSize().y() + 10;
	if(m_er->m_ertype != 2)
		canvas_size.y() += 10 + m_location->getSize().y();
	m_canvas->setSize(canvas_size);

	//set position
	m_category->setPosition(osg::Vec2f(10.0f, canvas_size.y()- m_category->getSize().y() - 10.0f));
	m_title->setPosition(osg::Vec2f(10.0f, m_category->getPosition().y() - m_title->getSize().y() - 10.0f));
	m_decription->setPosition(osg::Vec2f(10.0f, m_title->getPosition().y() - m_decription->getSize().y()));

	if(line_full){
		m_expand->setPosition(osg::Vec2f(180, m_decription->getPosition().y() + 8));
	}
	else{
		m_expand->setPosition(osg::Vec2f(-100,100));
	}

	if(m_er->m_ertype != 2)
		m_location->setPosition(osg::Vec2f(10.0f, m_decription->getPosition().y() - 10 - m_location->getSize().y()));
	else
		m_location->setPosition(osg::Vec2f(-100, -100));

	if(m_status == INFO_CARD_DISPLAY)
		canvas_pos.x() = 10;
	else
		canvas_pos.x() = -300;//hiding
	if(g_vsimapp->getNarrativeManager()->isBarOpen())
		canvas_pos.y() = 0.5*(g_vsimapp->getEResourcesManager()->BarHeight() - g_settings.m_bar_size) + rsize.y()/2 - 0.5*canvas_size.y();
	else
		canvas_pos.y() = 0.5*g_vsimapp->getEResourcesManager()->BarHeight() + rsize.y()/2 - 0.5*canvas_size.y();

	m_canvas->setPosition(canvas_pos);
	
	//set colors, font size
	erCategory current_category;
	float Red, Green, Blue;
	current_category = m_er->m_category;

	Red = current_category.Red/255.0;
	Green = current_category.Green/255.0;
	Blue = current_category.Blue/255.0;
//	m_canvas->setColor(osg::Vec4f(Red,Green,Blue,0.3f));
	m_category->setColor(osg::Vec4f(Red,Green,Blue,0.6f));
	m_category->setBorderColor(osg::Vec4f(1,1,1,0));
	m_category->setFontSize(14);
	m_title->setBorderColor(osg::Vec4f(1,1,1,0));
	m_title->setFontColor(osg::Vec4f(1.0,1.0,1.0,1.0f));
	m_title->setFontSize(16);
	m_decription->setBorderColor(osg::Vec4f(1,1,1,0));
	m_decription->setFontSize(12);
	m_expand->setFontSize(12);
	m_expand->setFontColor(osg::Vec4f(Red,Green,Blue,1.0f));
	m_expand->setBorderColor(osg::Vec4f(Red,Green,Blue,1.0f));
	m_location->setBorderColor(osg::Vec4f(1,1,1,0));
	m_location->setFontSize(12);
}


void EResourcesInfoCard::UpdateExpandedInfoCard(){

	assert(m_er != NULL);

	//set labels
	m_category->setLabel(m_er->getCategoryName());
	m_title->setLabel(m_er->getResourceName());
	m_expand->setLabel("LESS");
	std::string location;
	int num_line_location = 0;
	bool line_full = false;
	if(m_er->m_ertype != 2)//not annotation, there is a file location or url
	{
		Util::MultilineGen2(m_er->m_filepath, location, num_line_location, 34, 5, line_full);
		m_location->setLabel(location);
	}

	//multiple line
	std::string description;
	int num_line_description;
	Util::MultilineGen(m_er->m_description, description, num_line_description, 34, 20, line_full);
	description += "\n\n  ";
	num_line_description += 1;

	m_decription->setLabel(description);

	//set widget size
	osgNewWidget::Canvas* uiCanvas = osgNewWidget::WidgetManager::instance()->getUIRoot();
	osg::Vec2f rsize = uiCanvas->getSize();
	osg::Vec2f canvas_size, canvas_pos;

	m_category->setSize(osg::Vec2f(230, 26));
	m_title->setSize(osg::Vec2f(230, 36));
	m_expand->setSize(osg::Vec2f(52,20));
	m_decription->setSize(osg::Vec2f(230, num_line_description*25));

	if(m_er->m_ertype != 2)//not annotation
		m_location->setSize(osg::Vec2f(230, num_line_location*16));
	
	canvas_size.x() = 250;
	canvas_size.y() = m_title->getSize().y() + m_category->getSize().y() + 20 + m_decription->getSize().y() + 10;
	if(m_er->m_ertype != 2)
		canvas_size.y() += 10 + m_location->getSize().y();
	m_canvas->setSize(canvas_size);

	//set position
	m_category->setPosition(osg::Vec2f(10.0f, canvas_size.y()- m_category->getSize().y() - 10.0f));
	m_title->setPosition(osg::Vec2f(10.0f, m_category->getPosition().y() - m_title->getSize().y() - 10.0f));
	m_decription->setPosition(osg::Vec2f(10.0f, m_title->getPosition().y() - m_decription->getSize().y()));
	m_expand->setPosition(osg::Vec2f(180, m_decription->getPosition().y() + 8));

	if(m_er->m_ertype != 2)
		m_location->setPosition(osg::Vec2f(10.0f, m_decription->getPosition().y() - 10 - m_location->getSize().y()));
	else
		m_location->setPosition(osg::Vec2f(-100, -100));

	if(m_status == INFO_CARD_DISPLAY)
		canvas_pos.x() = 10;
	else
		canvas_pos.x() = -300;//hiding
	if(g_vsimapp->getNarrativeManager()->isBarOpen())
		canvas_pos.y() = 0.5*(g_vsimapp->getEResourcesManager()->BarHeight() - g_settings.m_bar_size) + rsize.y()/2 - 0.5*canvas_size.y();
	else
		canvas_pos.y() = 0.5*g_vsimapp->getEResourcesManager()->BarHeight() + rsize.y()/2 - 0.5*canvas_size.y();

	m_canvas->setPosition(canvas_pos);
	
	//set colors, font size
	erCategory current_category;
	float Red, Green, Blue;
	current_category = m_er->m_category;

	Red = current_category.Red/255.0;
	Green = current_category.Green/255.0;
	Blue = current_category.Blue/255.0;
//	m_canvas->setColor(osg::Vec4f(Red,Green,Blue,0.3f));
	m_category->setColor(osg::Vec4f(Red,Green,Blue,0.6f));

	m_category->setBorderColor(osg::Vec4f(1,1,1,0));
	m_category->setFontSize(14);
	m_title->setBorderColor(osg::Vec4f(1,1,1,0));
	m_title->setFontSize(16);
	m_title->setFontColor(osg::Vec4f(Red,Green,Blue,1.0f));
	m_decription->setBorderColor(osg::Vec4f(1,1,1,0));
	m_decription->setFontSize(12);
	m_expand->setFontSize(12);
	m_expand->setFontColor(osg::Vec4f(Red,Green,Blue,1.0f));
	m_expand->setBorderColor(osg::Vec4f(Red,Green,Blue,1.0f));
	m_location->setBorderColor(osg::Vec4f(1,1,1,0));
	m_location->setFontSize(12);
}

void EResourcesInfoCard::Respond2Resize(){
	osgNewWidget::Canvas* uiCanvas = osgNewWidget::WidgetManager::instance()->getUIRoot();
	osg::Vec2f rsize = uiCanvas->getSize();

	osg::Vec2f canvas_size, canvas_pos;
	canvas_size = m_canvas->getSize();
	if(m_status == INFO_CARD_DISPLAY)
		canvas_pos.x() = 10;
	else
		canvas_pos.x() = -300;//hiding
	if(g_vsimapp->getNarrativeManager()->isBarOpen())
		canvas_pos.y() = 0.5*(g_settings.m_bar_size_2 - g_settings.m_bar_size) + rsize.y()/2 - 0.5*canvas_size.y();
	else
		canvas_pos.y() = 0.5*g_settings.m_bar_size_2+ rsize.y()/2 - 0.5*canvas_size.y();

	m_canvas->setPosition(canvas_pos);
}

void EResourcesInfoCard::animationEnded(osgNewWidget::Widget* widget){

}
