/*
 * Label.h
 *
 *  Created on: May 4, 2011
 *      Author: eduardo
 */

#ifndef OSGNW_LABEL_H_
#define OSGNW_LABEL_H_

#include "osgNewWidget/Widget.h"
#include <osgText/Text>

namespace osgNewWidget
{

class Label: public Widget
{
public:
    Label(int flags = osgNewWidget::WIDGET_NONE);
    Label(const Label&, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~Label();

    META_Node(osgNewWidget, Label);

    std::string getLabel();
    virtual void setLabel(const std::string& label);
    virtual void setPosition(const osg::Vec2f& pos);
    virtual void setSize(const osg::Vec2f& size);
    void setFont(const std::string& font);
    void setFontSize(unsigned int size);
    osg::Vec4f getFontColor() const;
    void setFontColor(const osg::Vec4f& color);
    virtual void setAlphaMult(float a);
    void setShadow();
    osg::Vec2f getTextSize();
	osg::Vec2f getLetterSize(){
		return m_letter_size;};
    virtual void setZOffset(int zoffset);

	// Used only by serializer
    const osgText::Text* getText() const;
    void setText(osgText::Text* text);

protected:
    osg::ref_ptr<osgText::Text> m_text;
	osg::Vec2f m_letter_size;
    // "Real" label alpha is stored here, because the alpha values
    // of color and backdropColor in m_text may be animated.
    float m_labelAlpha;
    
    void update();
    void updateLabelAndBackgroundAlpha();
};

}

#endif /* OSGNW_LABEL_H_ */
