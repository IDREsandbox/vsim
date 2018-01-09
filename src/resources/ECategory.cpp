#include "resources/ECategory.h"

ECategory::ECategory()
	: Group(),
	m_cat_name("Untitled"),
	m_red(0),
	m_blue(0),
	m_green(0)
{

}
ECategory::ECategory(const ECategory& e, const osg::CopyOp& copyop)
	: Group(e, copyop),
	m_cat_name(e.m_cat_name),
	m_red(e.m_red),
	m_blue(e.m_blue),
	m_green(e.m_green)
{

}

ECategory::~ECategory()
{
}

const std::string& ECategory::getCategoryName() const
{
	return m_cat_name;
}
void ECategory::setCategoryName(const std::string& name)
{
	std::string old_name = m_cat_name;
	m_cat_name = name;
	emit sCNameChanged(old_name, name);
}

int ECategory::getRed() const
{
	return m_red;
}
void ECategory::setRed(int red)
{
	m_red = red;
}
int ECategory::getGreen() const
{
	return m_green;
}
void ECategory::setGreen(int green)
{
	m_green = green;
}
int ECategory::getBlue() const
{
	return m_blue;
}
void ECategory::setBlue(int blue)
{
	m_blue = blue;
}

QColor ECategory::getColor() const
{
	return QColor(m_red, m_green, m_blue);
}

void ECategory::setColor(QColor color)
{
	m_red = color.red();
	m_green = color.green();
	m_blue = color.blue();
	emit sColorChanged(color);
}
