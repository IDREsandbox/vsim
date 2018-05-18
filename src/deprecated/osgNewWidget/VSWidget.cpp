/*
 * VSWidget.cpp
 *
 *  Created on: May 8, 2011
 *	  Author: eduardo
 */
#include <cassert>
//#include <osgWidget/Util>
//#include <osgWidget/WindowManager>
//#include <osgWidget/Frame>
//#include <osgWidget/Box>
//#include <osgWidget/EventInterface>
#include "VSWidget.h"
#include <QDebug>
//#include "menu/Settings.h"


VSWidget::VSWidget(int flags): osgNewWidget::Widget(flags)
{
	//if (flags & osgNewWidget::WIDGET_WITH_GEOMETRY)
	//	setColor(g_settings.m_widget_bg_color);
	//if (flags & osgNewWidget::WIDGET_WITH_BORDER)
	//	setBorderColor(g_settings.m_widget_frame_color);
}

VSWidget::VSWidget(const VSWidget& widget, const osg::CopyOp& copyop)
	: osgNewWidget::Widget(widget, copyop)
{
}

VSWidget::~VSWidget()
{
}

// -----------------------------------------------------------------------------

VSLabel::VSLabel(int flags): osgNewWidget::Label(flags)
{
}

VSLabel::VSLabel(const std::string& label, int flags): osgNewWidget::Label(flags)
{
	//setLabel(label);
	//setFont("C:\\Users\\samin\\Desktop\\DroidSans.ttf");
	//setFont("T:\\Projects\\_UCLA\\vsim\\vsim\\nrts\\src\\VSim\\windows\\Release\\DroidSans.ttf"); 
	//setFontSize(16);
	//setFontColor(g_settings.m_widget_font_color);
	//if (flags & osgNewWidget::WIDGET_WITH_GEOMETRY)
	//	setColor(g_settings.m_widget_bg_color);
	//if (flags & osgNewWidget::WIDGET_WITH_BORDER)
	//	setBorderColor(g_settings.m_widget_frame_color);
	
	//setSize(osg::Vec2f(26.0f, 26.0f));
}

VSLabel::VSLabel(const VSLabel& widget, const osg::CopyOp& copyop):
	osgNewWidget::Label(widget, copyop)
{
}

VSLabel::~VSLabel()
{
}

void VSLabel::setLabel(const std::string& label)
{
	//if (m_text.get())
	//{
	//	osgNewWidget::Label::setLabel(label);
	//}
	//else
	//{
	//	//osgNewWidget::Label::setLabel(label);
	//	////setFont("C:\\Users\\samin\\Desktop\\DroidSans.ttf");
	//	//setFont("T:\\Projects\\_UCLA\\vsim\\vsim\\nrts\\src\\VSim\\windows\\Release\\DroidSans.ttf");
	//	//setFontSize(16);
	//	//setFontColor(g_settings.m_widget_font_color);
	//}
}

// -----------------------------------------------------------------------------

VSCanvas::VSCanvas(int flags): osgNewWidget::Canvas(flags)
{
	//if (flags & osgNewWidget::WIDGET_WITH_GEOMETRY)
	//	setColor(g_settings.m_widget_bg_color);
	//if (flags & osgNewWidget::WIDGET_WITH_BORDER)
	//	setBorderColor(g_settings.m_widget_frame_color);
}

VSCanvas::VSCanvas(const VSCanvas& widget, const osg::CopyOp& copyop)
	: osgNewWidget::Canvas(widget, copyop)
{
}

VSCanvas::~VSCanvas()
{
}
//
//// -----------------------------------------------------------------------------
//
//VSMulti::VSMulti(const osg::Vec2f& size) : VSCanvas(osgNewWidget::WIDGET_WITH_GEOMETRY), m_widgetSpacing(10.0f), m_scrollAmount(42.0f), m_draggingWidget(NULL)
//{
//	VSCanvas::setSize(size);
//	setColor(osg::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
//	m_subcanvas = new VSCanvas();
//	m_subcanvas->setBorderColor(g_settings.m_multi_widget_frame_color);
//	m_subcanvas->setSize(osg::Vec2f(size.x() - 70.0f, size.y() - 1.0f));
//	m_subcanvas->setPosition(osg::Vec2f(35.0f, 1.0f));
//	VSCanvas::addWidget(m_subcanvas.get());
//
//	m_widget_scroll_l = new VSLabel("<"); 
//
//	m_widget_scroll_l->setSize(osg::Vec2f(15.0f, 15.0f));
//	m_widget_scroll_l->setHandler((osgNewWidget::IWidgetEventHandler*)this);
//	m_widget_scroll_l->setPosition(osg::Vec2f(10.0f, floor((size.y() - 17.0f) * 0.5f)));
//	VSCanvas::addWidget(m_widget_scroll_l.get());
//
//	m_widget_scroll_r = new VSLabel(">");
//	m_widget_scroll_r->setSize(osg::Vec2f(15.0f, 15.0f));
//	m_widget_scroll_r->setHandler((osgNewWidget::IWidgetEventHandler*)this);
//	m_widget_scroll_r->setPosition(osg::Vec2f(size.x() - 10.0f - 17.0f, floor((size.y() - 17.0f) * 0.5f)));
//	VSCanvas::addWidget(m_widget_scroll_r.get());
//	setHAnchor(m_widget_scroll_r.get(), osgNewWidget::ANCHOR_R);
//	setHAnchor(m_subcanvas.get(), osgNewWidget::ANCHOR_LR);
//}
//
//VSMulti::~VSMulti()
//{
//}
//
//
////bool scrollWindow(osgWidget::Event& ev) {
////	osgWidget::Frame* frame = dynamic_cast<osgWidget::Frame*>(ev.getWindow());
////	if (!frame) return false;
////
////	osgWidget::Window::EmbeddedWindow* ew = dynamic_cast<osgWidget::Window::EmbeddedWindow*>(frame->getEmbeddedWindow());
////	if (!ew) return false;
////
////	const osgWidget::Quad& va = ew->getWindow()->getVisibleArea();
////
////	if (ev.getWindowManager()->isMouseScrollingUp() && va[1] != 0.0f)
////		ew->getWindow()->addVisibleArea(0, -20);
////
////	else if (va[1] <= (ew->getWindow()->getHeight() - ew->getHeight()))
////		ew->getWindow()->addVisibleArea(0, 20);
////
////	frame->update();
////	return true;
////}
//
//void VSMulti::addWidget(osgNewWidget::Widget* widget)
//{
//	osg::Vec2f nextPos(m_widgetSpacing, 0.0f);
//	osg::Group* group = m_subcanvas->getGroup();
//	if (group->getNumChildren() > 0)
//	{
//		osg::Node* n = group->getChild(group->getNumChildren() - 1);
//		osgNewWidget::Widget* w = dynamic_cast<osgNewWidget::Widget*>(n);
//		if (w)
//		{
//			nextPos = w->getPosition();
//			nextPos.x() += w->getSize().x() + m_widgetSpacing;
//		}
//	}
//	osg::Vec2f wpos = widget->getPosition();
//	osg::Vec2f wsize = widget->getSize();
//	osg::Vec2f csize = m_subcanvas->getSize();
//	wpos.x() = wpos.x() + nextPos.x();
//	wpos.y() = floor((csize.y() - wsize.y()) / 2.0f);
//	widget->setPosition(wpos);
//	m_subcanvas->addWidget(widget);
//}
//
//void VSMulti::addWidgetToPrev(osgNewWidget::Widget* widget)
//{
//	osg::Vec2f prevPos(0.0f, 0.0f);
//	osg::Group* group = m_subcanvas->getGroup();
//	assert(group->getNumChildren() > 0);
//	osg::Node* n = group->getChild(group->getNumChildren() - 1);
//	osgNewWidget::Widget* prevWidget = dynamic_cast<osgNewWidget::Widget*>(n);
//	
//	prevPos = prevWidget->getPosition();
//
//	osg::Vec2f wpos = widget->getPosition();
//	wpos += prevPos;
//	widget->setPosition(wpos);
//	widget->setZOffset(prevWidget->getZOffset() + 1);
//	m_subcanvas->addWidget(widget);
//
//	osg::ref_ptr<VSLabel> padding;
//	padding = new VSLabel("");
//	padding->setSize(prevWidget->getSize());
//	padding->setPosition(prevWidget->getPosition());
//	padding->setColor(osg::Vec4f(1,1,1,0));
//	padding->setBorderColor(osg::Vec4f(1,1,1,0));
//	m_subcanvas->addWidget(padding);
//}
//
//
//
//void VSMulti::removeWidget(osgNewWidget::Widget* widget)
//{
//	osg::Group* group = m_subcanvas->getGroup();
//	unsigned int index = group->getChildIndex(widget);
//	if (index == group->getNumChildren())
//		return;
//	osg::Vec2f nextPos(m_widgetSpacing, 0.0f);
//	if (index > 0)
//	{
//		osg::Node* n = group->getChild(index - 1);
//		osgNewWidget::Widget* w = dynamic_cast<osgNewWidget::Widget*>(n);
//		if (w)
//		{
//			nextPos = w->getPosition();
//			nextPos.x() += w->getSize().x() + m_widgetSpacing;
//		}
//	}
//	for (unsigned int i = index + 1; i < group->getNumChildren(); i++)
//	{
//		osg::Node* n = group->getChild(i);
//		osgNewWidget::Widget* w = dynamic_cast<osgNewWidget::Widget*>(n);
//		if (w)
//		{
//			w->setPosition(nextPos);
//			nextPos.x() += w->getSize().x() + m_widgetSpacing;
//		}
//	}
//	m_subcanvas->removeWidget(widget);
//}
//
//void VSMulti::clear()
//{
//	osg::Group* group = m_subcanvas->getGroup();
//	group->removeChildren(0, group->getNumChildren());
//}
//
//void VSMulti::setSize(const osg::Vec2f& size)
//{
//	Canvas::setSize(size);
//	m_widget_scroll_l->setPosition(osg::Vec2f(10.0f, floor((size.y() - 17.0f) * 0.5f)));
//	float rpos_x = m_widget_scroll_r->getPosition().x();
//	m_widget_scroll_r->setPosition(osg::Vec2f(rpos_x, floor((size.y() - 17.0f) * 0.5f)));
//}
//
//void VSMulti::handleDrag(osgNewWidget::Widget* widget, const osgNewWidget::WidgetEvent& ev){
//	m_mousePos = ev.pos;
//	m_draggingWidget = widget;
//
//	if (m_draggingWidget != NULL && widget == NULL)
//	{
//		//OSG_ALWAYS << "Drag" << std::endl;
//		osg::Vec2f pos = m_draggingWidget->getPosition();
//		pos += ev.pos - m_mousePos;
//		m_draggingWidget->setPosition(pos);
//		m_mousePos = ev.pos;
//	}
//
//	//if (m_resizingWidget != NULL)
//	//{
//	//	float minsize = 20.0f;
//	//	osg::Vec2f delta = event.pos - m_mousePos;
//	//	osg::Vec2f pos = m_resizingWidget->getPosition();
//	//	osg::Vec2f size = m_resizingWidget->getSize();
//	//	osg::Vec2f prevUpperLeft = pos + osg::Vec2f(0.0f, size.y());
//	//	size += osg::Vec2f(delta.x(), -delta.y());
//
//	//	if (size.x() < minsize)
//	//		size.x() = minsize;
//	//	if (size.y() < minsize)
//	//		size.y() = minsize;
//
//	//	if (m_resizingWidget->getName() == "overlay_Image"){
//	//		//fix ratio
//	//		osg::Vec2f imagesize = m_resizingWidget->getSize();
//	//		double ratio = imagesize.y() / imagesize.x();
//	//		size.y() = ratio*size.x();
//	//	}
//
//	//	m_resizingWidget->setSize(size);
//	//	m_resizingWidget->setPosition(prevUpperLeft - osg::Vec2f(0.0f, size.y()));
//	//	m_mousePos = event.pos;
//
//	//	VSLabel* text;
//	//	text = dynamic_cast<VSLabel*>(m_resizingWidget);
//	//	if (text){
//	//		std::string oldtext, newtext;
//	//		oldtext = text->getLabel();
//
//	//		osg::Vec2f lettersize = text->getLetterSize();
//	//		int line_capacity = (size.x() - 20) / lettersize.x();
//	//		line_capacity = std::max(line_capacity, 10);
//	//		int num_line; bool line_full;
//	//		Util::MultilineGen3(oldtext, newtext, num_line, line_capacity, 200, line_full, 1);
//	//		text->setLabel(newtext);
//	//	}
//	//}
//
//}
//
//void VSMulti::handle(osgNewWidget::Widget* widget, const osgNewWidget::WidgetEvent& ev)
//{
//	/*if (ev.type != osgNewWidget::WidgetEvent::MOUSE_PUSH)
//		return;*/
//
//	//m_mousePos = ev.pos;
//	//m_draggingWidget = widget;
//
//	//if (ev.type == osgNewWidget::WidgetEvent::MOUSE_DRAG){
//	//	osg::Vec2f pos = m_draggingWidget->getPosition();
//	//	pos += ev.pos - m_mousePos;
//	//	m_draggingWidget->setPosition(pos);
//	//	m_mousePos = ev.pos;
//	//}
//
//	/*if (ev.type == osgNewWidget::WidgetEvent::MOUSE_RELEASE){
//		m_draggingWidget = NULL;
//	}*/
//	//	osg::Vec2f offset = m_subcanvas->getContentOffset();
//	//	if (offset.x() < 0.0f)
//	//	{
//	//		offset.x() += m_scrollAmount;
//	//		//m_subcanvas->setContentOffset(offset);
//	//		osgNewWidget::WidgetAnimator::instance()->add(m_subcanvas, osgNewWidget::ANIM_PROPERTY_CONTENT_OFFSET_X,
//	//			0.0f, 0.33f, offset.x(), NULL);
//	//	}
//	//}
//	//
//	//if (ev.type == osgNewWidget::WidgetEvent::MOUSE_SCROLL_DOWN){
//	//	osg::Vec2f offset = m_subcanvas->getContentOffset();
//	//	osg::Vec2f nextPos(m_widgetSpacing, 0.0f);
//	//	osg::Group* group = m_subcanvas->getGroup();
//	//	if (group->getNumChildren() > 0)
//	//	{
//	//		osg::Node* n = group->getChild(group->getNumChildren() - 1);
//	//		osgNewWidget::Widget* w = dynamic_cast<osgNewWidget::Widget*>(n);
//	//		if (w)
//	//		{
//	//			nextPos = w->getPosition();
//	//			nextPos.x() += w->getSize().x() + m_widgetSpacing;
//	//		}
//	//	}
//	//	if (offset.x() > -(nextPos.x() - m_subcanvas->getSize().x()))
//	//	{
//	//		offset.x() -= m_scrollAmount;
//	//		//m_subcanvas->setContentOffset(offset);
//	//		osgNewWidget::WidgetAnimator::instance()->add(m_subcanvas, osgNewWidget::ANIM_PROPERTY_CONTENT_OFFSET_X,
//	//			0.0f, 0.33f, offset.x(), NULL);
//	//	}
//	//}
//
//	//if (ev.type == osgNewWidget::WidgetEvent::MOUSE_SCROLL_UP)
//	//{
//	//	osg::Vec2f offset = m_subcanvas->getContentOffset();
//	//	if (offset.x() < 0.0f)
//	//	{
//	//		offset.x() += m_scrollAmount;
//	//		//m_subcanvas->setContentOffset(offset);
//	//		osgNewWidget::WidgetAnimator::instance()->add(m_subcanvas, osgNewWidget::ANIM_PROPERTY_CONTENT_OFFSET_X,
//	//			0.0f, 0.33f, offset.x(), NULL);
//	//	}
//	//}
//
//	if (widget == m_widget_scroll_l.get())
//	{
//		osg::Vec2f offset = m_subcanvas->getContentOffset();
//		if (offset.x() < 0.0f)
//		{
//			offset.x() += m_scrollAmount;
//			//m_subcanvas->setContentOffset(offset);
//			osgNewWidget::WidgetAnimator::instance()->add(m_subcanvas, osgNewWidget::ANIM_PROPERTY_CONTENT_OFFSET_X,
//				0.0f, 0.33f, offset.x(), NULL);
//		}
//	}
//	else if (widget == m_widget_scroll_r.get())
//	{
//		osg::Vec2f offset = m_subcanvas->getContentOffset();
//		osg::Vec2f nextPos(m_widgetSpacing, 0.0f);
//		osg::Group* group = m_subcanvas->getGroup();
//		if (group->getNumChildren() > 0)
//		{
//			osg::Node* n = group->getChild(group->getNumChildren() - 1);
//			osgNewWidget::Widget* w = dynamic_cast<osgNewWidget::Widget*>(n);
//			if (w)
//			{
//				nextPos = w->getPosition();
//				nextPos.x() += w->getSize().x() + m_widgetSpacing;
//			}
//		}
//		if (offset.x() > -(nextPos.x() - m_subcanvas->getSize().x()))
//		{
//			offset.x() -= m_scrollAmount;
//			//m_subcanvas->setContentOffset(offset);
//			osgNewWidget::WidgetAnimator::instance()->add(m_subcanvas, osgNewWidget::ANIM_PROPERTY_CONTENT_OFFSET_X,
//				0.0f, 0.33f, offset.x(), NULL);
//		}
//	}
//
//}
