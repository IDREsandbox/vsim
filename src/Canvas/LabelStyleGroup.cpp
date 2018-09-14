#include "Canvas/LabelStyleGroup.h"
#include <QDebug>

#include "Canvas/LabelStyle.h"

LabelStyleGroup::LabelStyleGroup()
{
	m_h1 = std::make_unique<LabelStyle>(LabelType::HEADER1, "Arial", 28, QColor(255, 255, 255), QColor(0, 0, 0, 178),
		QFont::Bold, false, Qt::AlignCenter, 13);
	m_h1->m_size = QSize(500, 70);
	m_h1->frameStyle()->m_frame_color = QColor(0, 0, 0);
	m_h1->frameStyle()->m_has_frame = false;
	m_h1->frameStyle()->m_frame_width = 0;

	m_h2 = std::make_unique<LabelStyle>(LabelType::HEADER2, "Arial", 18, QColor(0, 84, 166), QColor(255, 255, 255, 178),
		QFont::Bold, false, Qt::AlignCenter, 2);
	m_h2->m_size = QSize(300, 40);
	m_h2->frameStyle()->m_frame_color = QColor(0, 0, 0);
	m_h2->frameStyle()->m_has_frame = false;
	m_h2->frameStyle()->m_frame_width = 0;

	m_bod = std::make_unique<LabelStyle>(LabelType::BODY, "Arial", 10, QColor(0, 0, 0), QColor(255, 255, 255, 178),
		QFont::Normal, false, Qt::AlignLeft | Qt::AlignTop, 10);
	m_bod->m_size = QSize(175, 200);
	m_bod->frameStyle()->m_frame_color = QColor(0, 0, 0);
	m_bod->frameStyle()->m_has_frame = false;
	m_bod->frameStyle()->m_frame_width = 0;

	m_lab = std::make_unique<LabelStyle>(LabelType::LABEL, "Arial", 10, QColor(255, 255, 255), QColor(0, 84, 166),
		QFont::Bold, false, Qt::AlignCenter, 4);
	m_lab->m_size = QSize(125, 25);
	m_lab->frameStyle()->m_frame_color = QColor(0, 0, 0);
	m_lab->frameStyle()->m_has_frame = false;
	m_lab->frameStyle()->m_frame_width = 0;

	m_image = std::make_unique<FrameStyle>();
	m_image->m_bg_color = QColor(0, 0, 0);
	m_image->m_frame_color = QColor(255, 255, 255);
	m_image->m_has_frame = true;
	m_image->m_frame_width = 5;
}

LabelStyleGroup::~LabelStyleGroup()
{
}

void LabelStyleGroup::copy(const LabelStyleGroup & other)
{
	m_h1->copy(other.m_h1.get());
	m_h2->copy(other.m_h2.get());
	m_bod->copy(other.m_bod.get());
	m_lab->copy(other.m_lab.get());
	*m_image = *other.m_image;
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
