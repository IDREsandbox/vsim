/*
 * EResourcesList.cpp
 *
 *  Created on: 9/27/2012
 *      Author: Franklin Fang
 */

#ifndef E_RESOURCESFILTER_H_
#define E_RESOURCESFILTER_H_
#include "osgNewWidget/VSWidget.h"	

class EResourcesListEditor;
class EResourcesList;
struct FilterTag{
public:
	FilterTag(){m_select = false;}
	osg::ref_ptr<VSLabel> m_widget;
	bool m_select;
};

typedef std::vector<FilterTag*> FilterTagVector;

class EResourcesFilter:public osgNewWidget::IWidgetEventHandler{
public:
	EResourcesFilter();
	virtual ~EResourcesFilter();
	void init(osg::ref_ptr<VSCanvas> a_root_canvas, osg::ref_ptr<EResourcesListEditor> a_ere_list_editor);
	void applyFilter(osg::ref_ptr<EResourcesList> original_list, osg::ref_ptr<EResourcesList> target_list);//take a list, return a filtered  list
	void setWidgets();
	void setWidgetPositions();
	bool IsFilterOn();
private:

	void handle(osgNewWidget::Widget* widget, const osgNewWidget::WidgetEvent& ev);
	void handleMouseMove(osgNewWidget::Widget* widget);
	void handleMouseLeftClick(osgNewWidget::Widget* widget);
	char GetInitial(std::string name);
	osg::ref_ptr<VSLabel> m_label_title;
	osg::ref_ptr<VSLabel> m_label_category;
	osg::ref_ptr<VSLabel> m_label_filetype;

	osg::ref_ptr<VSCanvas> m_root_canvas;

	int m_filter_title_count;
	int m_filter_type_count;
	int m_filter_category_count;
	FilterTagVector m_tags_vector_title;
	FilterTagVector m_tags_vector_category;
	FilterTagVector m_tags_vector_filetype;
	osg::ref_ptr<EResourcesListEditor> m_ere_list_editor;

};

#endif