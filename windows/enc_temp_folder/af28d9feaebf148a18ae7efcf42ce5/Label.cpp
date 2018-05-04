/*
 * Label.cpp
 *
 *  Created on: May 4, 2011
 *	Author: eduardo
 */

#include "deprecated/osgNewWidget/Label.h"
#include <QDebug>
namespace osgNewWidget
{

Label::Label(int flags) : 
	Widget(flags),
	m_text(NULL),
	m_labelAlpha(1.0f)
{
	//qDebug() << "null mtext";
		//std::string temp;
		//temp = getLabel();
		//setLabel("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		//m_letter_size = getTextSize();
		//m_letter_size.x() /= 100;
		//setLabel(temp);
}

Label::Label(const Label& label, const osg::CopyOp& copyop)
	: Widget(label, copyop),
	m_text(label.m_text),
	m_labelAlpha(label.m_labelAlpha)
{
	//qDebug() << "mtext copyop";
		//std::string temp;
		//temp = getLabel();
		//setLabel("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		//m_letter_size = getTextSize();
		//m_letter_size.x() /= 100;
		//setLabel(temp);
}

Label::~Label()
{
}

std::string Label::getLabel()
{
	if (m_text.get())
			 return m_text->getText().createUTF8EncodedString();
	else
		return "";
}

void Label::setLabel(const std::string& label)
{
	if (m_text.get()){
		m_text->setText(label);
	}
	else
	{
		m_text = new osgText::Text();
		m_text->setText(label);
		m_text->setAlignment(osgText::Text::LEFT_BOTTOM);
		m_text->setDataVariance(osg::Object::DYNAMIC);
		addDrawable(m_text);
	}
	update();
}

// Used only by serializer.
const osgText::Text* Label::getText() const
{
	//OSG_ALWAYS << "getText" << std::endl;
	// Update the object's alpha so that it's saved correctly.
	osgNewWidget::Label* mutableThis = const_cast<osgNewWidget::Label*>(this);
	mutableThis->setAlphaMult(1.0f);
	return m_text;
}

// Used only by serializer.
void Label::setText(osgText::Text* text)
{
	//std::string s = text->getText().createUTF8EncodedString();
	//qDebug() << "set text" << QString::fromStdString(s);
	//OSG_ALWAYS << "setText" << std::endl;
	m_text = text;
	m_labelAlpha = m_text->getColor().a();
}

void Label::setPosition(const osg::Vec2f& pos)
{
	Widget::setPosition(pos);
	update();
}

void Label::setSize(const osg::Vec2f& size)
{
	Widget::setSize(size);
	update();
}

void Label::update()
{
	//if (!m_text.get())
	//	return;
	//osg::BoundingBox bb = m_text->getBoundingBox();
	//osg::Vec2f textSize(osg::round(bb.xMax() - bb.xMin()), osg::round(bb.yMax() - bb.yMin()));

	//osg::Vec3f textPos;
	//textPos.z() = (getZOffset() + 1) * 0.01f;
	//osg::Vec2f size = getSize();
	//osg::Vec2f pos = getPosition();
	//textPos.x() = osg::round((size.x() - textSize.x()) / 2.0f + pos.x()) + m_parentOffset.x(); 
	//textPos.y() = osg::round((size.y() - textSize.y()) / 2.0f + pos.y()) + m_parentOffset.y(); 
	//m_text->setPosition(textPos);
	//m_text->dirtyBound();

}

void Label::setFont(const std::string& font)
{
	//if (!m_text.get())
	//	return;
	//m_text->setFont(font);
	//
	//std::string temp;
	//temp = getLabel();
	//setLabel("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	//m_letter_size = getTextSize();
	//m_letter_size.x() /= 100;
	//setLabel(temp);
	//
	//update();
}

void Label::setFontSize(unsigned int size)
{
	//if (!m_text.get())
	//	return;
	//m_text->setCharacterSize(size);
	//m_text->setFontResolution(size, size);

	//std::string temp;
	//temp = getLabel();
	//setLabel("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	//m_letter_size = getTextSize();
	//m_letter_size.x() /= 100;
	//setLabel(temp);

	//update();
}

osg::Vec4f Label::getFontColor() const
{
	if (!m_text.get())
		return osg::Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
	osg::Vec4f color = m_text->getColor();
	// Publicly visible color must have the original alpha, not the (possibly animated) current one
	color.a() = m_labelAlpha;
	return color;
}

void Label::setFontColor(const osg::Vec4f& color)
{
	if (!m_text.get())
		return;
	m_text->setColor(color);
	m_labelAlpha = color.a();
}

void Label::setShadow()
{
	if (!m_text.get())
		return;
	m_text->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_RIGHT);
	m_text->setBackdropColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	m_text->setBackdropImplementation(osgText::Text::STENCIL_BUFFER);
	m_text->setBackdropOffset(0.1f);
}

osg::Vec2f Label::getTextSize()
{
	if (!m_text.get())
		return osg::Vec2f(0.0f, 0.0f);
	osg::BoundingBox bb;
	bb.expandBy( m_text->getBound() );
	return osg::Vec2f(bb.xMax() - bb.xMin(), bb.yMax() - bb.yMin());
}

void Label::setZOffset(int zoffset)
{
	Widget::setZOffset(zoffset);
	if (!m_text.get())
		return;
	osg::Vec3f textPos = m_text->getPosition();
	textPos.z() = (getZOffset() + 1) * 0.01f;
	m_text->setPosition(textPos);
	m_text->dirtyBound();
}

void Label::setAlphaMult(float a)
{
	//OSG_ALWAYS << "Label " << m_text->getText().createUTF8EncodedString() << " amult=" << a <<
	//	" alpha=" << m_alpha << std::endl;
	Widget::setAlphaMult(a);
	updateLabelAndBackgroundAlpha();
}

void Label::updateLabelAndBackgroundAlpha()
{
	if (!m_text.get())
		return;
	osg::Vec4f c;
	c = m_text->getColor();
	c.a() = m_labelAlpha * m_alphaMult;
	m_text->setColor(c);
	c = m_text->getBackdropColor();
	c.a() = m_labelAlpha * m_alphaMult;
	m_text->setBackdropColor(c);
}

}
