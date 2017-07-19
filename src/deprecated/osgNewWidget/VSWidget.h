/*
 * VSWidget.h
 *
 *  Created on: May 8, 2011
 *      Author: eduardo
 */

#ifndef VSWIDGET_H_
#define VSWIDGET_H_

#include "deprecated/osgNewWidget/Widget.h"
#include "deprecated/osgNewWidget/Label.h"
#include "deprecated/osgNewWidget/Canvas.h"
#include <string>

class VSWidget: public osgNewWidget::Widget
{
public:
    VSWidget(int flags = osgNewWidget::WIDGET_NONE);
    VSWidget(const VSWidget&, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~VSWidget();

    META_Node(, VSWidget)
};

class VSLabel: public osgNewWidget::Label
{
public:
    VSLabel(int flags = osgNewWidget::WIDGET_NONE);
    VSLabel(const std::string& label,
        int flags = osgNewWidget::WIDGET_WITH_GEOMETRY | osgNewWidget::WIDGET_WITH_BORDER);
    VSLabel(const VSLabel&, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~VSLabel();

    virtual void setLabel(const std::string& label);

    META_Node(, VSLabel)
    /*
    virtual osg::Object* cloneType() const { return new VSLabel ("", osgNewWidget::WIDGET_NONE); }
    virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new VSLabel (*this,copyop); }
    virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const VSLabel *>(obj)!=0; }
    virtual const char* className() const { return "VSLabel"; }
    virtual const char* libraryName() const { return ""; }
    virtual void accept(osg::NodeVisitor& nv) { if (nv.validNodeMask(*this)) { nv.pushOntoNodePath(this); nv.apply(*this); nv.popFromNodePath(); } }
    */
};

class VSCanvas: public osgNewWidget::Canvas
{
public:
    VSCanvas(int flags = osgNewWidget::WIDGET_WITH_GEOMETRY | osgNewWidget::WIDGET_WITH_BORDER);
    VSCanvas(const VSCanvas&, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~VSCanvas();

    //META_Node(, VSCanvas)

	//virtual bool scrollWindow(osgWidget::Event& ev);
	//virtual void addCallback(Callback* call);
    virtual osg::Object* cloneType() const { return new VSCanvas (osgNewWidget::WIDGET_NONE); }
    virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new VSCanvas (*this,copyop); }
    virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const VSCanvas *>(obj)!=0; }
    virtual const char* className() const { return "VSCanvas"; }
    virtual const char* libraryName() const { return ""; }
    virtual void accept(osg::NodeVisitor& nv) { if (nv.validNodeMask(*this)) { nv.pushOntoNodePath(this); nv.apply(*this); nv.popFromNodePath(); } }
};

//class VSMulti: public VSCanvas, public osgNewWidget::IWidgetEventHandler
//{
//public:
//    VSMulti(const osg::Vec2f& size);
//    virtual ~VSMulti();
//	//bool scrollWindow(osgWidget::Event& ev);
//    virtual void addWidget(osgNewWidget::Widget* widget);
//	void addWidgetToPrev(osgNewWidget::Widget* widget);
//    virtual void removeWidget(osgNewWidget::Widget* widget);
//    void clear();
//    virtual void handle(osgNewWidget::Widget* widget, const osgNewWidget::WidgetEvent& ev);
//	void handleDrag(osgNewWidget::Widget* widget, const osgNewWidget::WidgetEvent& ev);
//    virtual void setSize(const osg::Vec2f& size);
//    void setScrollAmount(float value) { m_scrollAmount = value; }
//	float getScrollAmount(void){return m_scrollAmount;}
//
//	osg::Vec2f m_mousePos;
//
//    osg::ref_ptr<VSCanvas> m_subcanvas;
//    osg::ref_ptr<VSLabel> m_widget_scroll_l;
//    osg::ref_ptr<VSLabel> m_widget_scroll_r;
//	osgNewWidget::Widget* m_draggingWidget;
//    float m_widgetSpacing;
//    float m_scrollAmount;
//};



#endif /* VSWIDGET_H_ */
