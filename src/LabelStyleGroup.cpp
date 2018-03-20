#include "LabelStyleGroup.h"
#include <QDebug>

#include "LabelStyle.h"

LabelStyleGroup::LabelStyleGroup()
{
}

void LabelStyleGroup::init()
{
	if (getNumChildren() >= 4) {
		// TODO: error checking here?
		m_h1 = dynamic_cast<LabelStyle*>(child(0));
		m_h2 = dynamic_cast<LabelStyle*>(child(1));
		m_bod = dynamic_cast<LabelStyle*>(child(2));
		m_lab = dynamic_cast<LabelStyle*>(child(3));
	}
	else {
		removeChildren(0, getNumChildren());
		m_h1 = new LabelStyle();

		// w/h defaults
		//650, 80
		//250, 35
		//250, 255
		//175, 25
		m_h1 = new LabelStyle(LabelType::HEADER1, "Arial", 36, QColor(255, 255, 255, 255), QColor(0, 0, 0, 178),
			QFont::Bold, false, Qt::AlignCenter, 13);
		m_h2 = new LabelStyle(LabelType::HEADER2, "Arial", 20, QColor(244, 147, 31, 255), QColor(0, 229, 250, 35),
			QFont::Bold, false, Qt::AlignCenter, 2);
		m_bod = new LabelStyle(LabelType::BODY, "Arial", 12, QColor(255, 255, 255, 255), QColor(0, 0, 0, 178),
			QFont::Normal, false, Qt::AlignLeft | Qt::AlignTop, 10);
		m_lab = new LabelStyle(LabelType::LABEL, "Arial", 12, QColor(0, 0, 0, 255), QColor(255, 255, 255, 255),
			QFont::Bold, false, Qt::AlignCenter, 4);

		addChild(m_h1);
		addChild(m_h2);
		addChild(m_bod);
		addChild(m_lab);
	}
}

LabelStyle * LabelStyleGroup::getStyle(LabelType style) const
{
	// set style
	switch (style) {
	case LabelType::HEADER1:
		return m_h1;
		break;
	case LabelType::HEADER2:
		return m_h2;
		break;
	case LabelType::BODY:
		return m_bod;
		break;
	case LabelType::LABEL:
		return m_lab;
		break;
	}

	return nullptr;
}
