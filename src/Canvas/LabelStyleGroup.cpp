#include "Canvas/LabelStyleGroup.h"
#include <QDebug>

#include "Canvas/LabelStyle.h"

LabelStyleGroup::LabelStyleGroup(QObject *parent)
	: QObject(parent)
{
	m_h1 = std::make_unique<LabelStyle>(LabelType::HEADER1, "Arial", 36, QColor(255, 255, 255, 255), QColor(0, 0, 0, 178),
		QFont::Bold, false, Qt::AlignCenter, 13);
	m_h1->frameStyle()->m_frame_color = QColor(0, 0, 0);
	m_h1->frameStyle()->m_has_frame = true;
	m_h1->frameStyle()->m_frame_width = 0;

	m_h2 = std::make_unique<LabelStyle>(LabelType::HEADER2, "Arial", 20, QColor(244, 147, 31, 255), QColor(0, 229, 250, 35),
		QFont::Bold, false, Qt::AlignCenter, 2);
	m_h2->frameStyle()->m_frame_color = QColor(0, 0, 0);
	m_h2->frameStyle()->m_has_frame = true;
	m_h2->frameStyle()->m_frame_width = 0;

	m_bod = std::make_unique<LabelStyle>(LabelType::BODY, "Arial", 12, QColor(255, 255, 255, 255), QColor(0, 0, 0, 178),
		QFont::Normal, false, Qt::AlignLeft | Qt::AlignTop, 10);
	m_bod->frameStyle()->m_frame_color = QColor(0, 0, 0);
	m_bod->frameStyle()->m_has_frame = true;
	m_bod->frameStyle()->m_frame_width = 0;

	m_lab = std::make_unique<LabelStyle>(LabelType::LABEL, "Arial", 12, QColor(0, 0, 0, 255), QColor(255, 255, 255, 255),
		QFont::Bold, false, Qt::AlignCenter, 4);
	m_lab->frameStyle()->m_frame_color = QColor(0, 0, 0);
	m_lab->frameStyle()->m_has_frame = true;
	m_lab->frameStyle()->m_frame_width = 0;

	m_image = std::make_unique<FrameStyle>();
	m_image->m_bg_color = QColor(0, 0, 0);
	m_image->m_frame_color = QColor(0, 0, 0);
	m_image->m_frame_width = 1;
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

FrameStyle * LabelStyleGroup::getImageStyle() const
{
	return m_image.get();
}
