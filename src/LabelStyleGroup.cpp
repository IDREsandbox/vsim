#include "LabelStyleGroup.h"
#include <QDebug>

LabelStyleGroup::LabelStyleGroup()
{


	//addChild(m_h1);
	//addChild(m_h2);
	//addChild(m_bod);
	//addChild(m_lab);

	//m_img = new LabelStyle("\"Arial\"", 12, 0, 0, 0, 255, false, 255,
	//	255, 255, 255, 455, 325, "Italic", "Center", 10);
}

void LabelStyleGroup::init()
{
	qDebug() << "initializing style group";
	if (getNumChildren() >= 4) {
		qDebug() << "- group already has children";
		// TODO: error checking here?
		m_h1 = dynamic_cast<LabelStyle*>(child(0));
		m_h2 = dynamic_cast<LabelStyle*>(child(1));
		m_bod = dynamic_cast<LabelStyle*>(child(2));
		m_lab = dynamic_cast<LabelStyle*>(child(3));
	}
	else {
		qDebug() << "- creating new styles";
		removeChildren(0, getNumChildren());
		m_h1 = new LabelStyle("\"Arial\"", 36, 255, 255, 255, 255, false, 0,
			0, 0, 178, 650, 80, QFont::Bold, false, Qt::AlignHCenter, 13);
		m_h2 = new LabelStyle("\"Arial\"", 20, 244, 147, 31, 255, false, 0,
			0, 0, 229, 250, 35, QFont::Bold, false, Qt::AlignHCenter, 2);
		m_bod = new LabelStyle("\"Arial\"", 12, 255, 255, 255, 255, false, 0,
			0, 0, 178, 250, 255, QFont::Weight::Normal, false, Qt::AlignLeft, 10);
		m_lab = new LabelStyle("\"Arial\"", 12, 0, 0, 0, 255, false, 255,
			255, 255, 255, 175, 25, QFont::Bold, false, Qt::AlignHCenter, 4);
		addChild(m_h1);
		addChild(m_h2);
		addChild(m_bod);
		addChild(m_lab);
	}
}

LabelStyle * LabelStyleGroup::getStyle(LabelStyle::Style style) const
{
	// set style
	switch (style) {
	case LabelStyle::HEADER1:
		return m_h1;
		break;
	case LabelStyle::HEADER2:
		return m_h2;
		break;
	case LabelStyle::BODY:
		return m_bod;
		break;
	case LabelStyle::LABEL:
		return m_lab;
		break;
	}

	return nullptr;
}
