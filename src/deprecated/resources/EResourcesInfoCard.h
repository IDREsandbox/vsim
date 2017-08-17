/*
 * EResourcesInfoCard.h
 *
 *  Created on: 10 25, 2012
 *      Author: Franklin Fang
 */

#ifndef ERESOURCESINFOCARD_H_
#define ERESOURCESINFOCARD_H_

#include "osgNewWidget/VSWidget.h"
#include "resources/EResourcesNode.h"

enum InfoCardStatus{
	INFO_CARD_DISPLAY,
	INFO_CARD_HIDE
};

class EResourcesInfoCard:public osgNewWidget::IWidgetAnimationCallback,public osgNewWidget::IWidgetEventHandler{

public: 
	EResourcesInfoCard();
	~EResourcesInfoCard();
	
	static EResourcesInfoCard* Instance();
	void Init();
	void SetEResources(osg::ref_ptr<EResourcesNode> er){assert(er!= NULL);m_er = er;}
	void ShowInfoCard(osg::ref_ptr<EResourcesNode>);
	void ShowExpandedInfoCard(osg::ref_ptr<EResourcesNode>);
	void HideInfoCard();
	void Respond2Resize();
	
	osg::ref_ptr<EResourcesNode> GetCurrentER(){return m_er;};

private:
	void UpdateInfoCard();	
	void UpdateExpandedInfoCard();	
	void handle(osgNewWidget::Widget* widget, const osgNewWidget::WidgetEvent& ev);
	virtual void animationEnded(osgNewWidget::Widget* widget);
	osg::ref_ptr<VSLabel> m_title;
	osg::ref_ptr<VSLabel> m_category;
	osg::ref_ptr<VSLabel> m_decription;
	osg::ref_ptr<VSLabel> m_location;
	osg::ref_ptr<VSLabel> m_author;
	osg::ref_ptr<VSLabel> m_expand;
	osg::ref_ptr<VSCanvas> m_canvas;
	InfoCardStatus m_status;
	osg::ref_ptr<EResourcesNode> m_er;
	bool m_expanded;
}
;
#endif