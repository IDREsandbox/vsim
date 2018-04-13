#include "LabelStyleGroup.h"
#include <QDebug>

#include "LabelStyle.h"

LabelStyleGroup::LabelStyleGroup()
{
	m_h1 = std::unique_ptr<LabelStyle>(new LabelStyle(LabelType::HEADER1, "Arial", 36, QColor(255, 255, 255, 255), QColor(0, 0, 0, 178),
		QFont::Bold, false, Qt::AlignCenter, 13));
	m_h2 = std::unique_ptr<LabelStyle>(new LabelStyle(LabelType::HEADER2, "Arial", 20, QColor(244, 147, 31, 255), QColor(0, 229, 250, 35),
		QFont::Bold, false, Qt::AlignCenter, 2));
	m_bod = std::unique_ptr<LabelStyle>(new LabelStyle(LabelType::BODY, "Arial", 12, QColor(255, 255, 255, 255), QColor(0, 0, 0, 178),
		QFont::Normal, false, Qt::AlignLeft | Qt::AlignTop, 10));
	m_lab = std::unique_ptr<LabelStyle>(new LabelStyle(LabelType::LABEL, "Arial", 12, QColor(0, 0, 0, 255), QColor(255, 255, 255, 255),
		QFont::Bold, false, Qt::AlignCenter, 4));
}

LabelStyleGroup::~LabelStyleGroup()
{
}

LabelStyle * LabelStyleGroup::getStyle(LabelType style) const
{
	// set style
	switch (style) {
	case LabelType::HEADER1:
		return m_h1.get();
		break;
	case LabelType::HEADER2:
		return m_h2.get();
		break;
	case LabelType::BODY:
		return m_bod.get();
		break;
	case LabelType::LABEL:
		return m_lab.get();
		break;
	}

	return nullptr;
}
